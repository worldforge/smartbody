/*
 *  sk_scene.cpp - part of Motion Engine and SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcelo Kallmann, USC (currently UC Merced)
 *      Andrew n marshall, USC
 */
 
#include <sr/sr_lines.h>
#include <sr/sr_model.h>
#include <sr/sr_sphere.h>
#include <sr/sr_cylinder.h>
#include <sr/sr_sn_matrix.h>
#include <sr/sr_sn_shape.h>

#include <sk/sk_scene.h>
#include <sk/sk_skeleton.h>
#include <sk/sk_joint.h>
#include "SBUtilities.h"



# define DEF_CYL_RADIUS 0.5f
# define DEF_SPH_FACTOR 1.6f;
# define DEF_AXIS_LEN   1.8f


//============================= SkScene ============================

SkScene::SkScene ()
 {
   _cradius = DEF_CYL_RADIUS;
   _sfactor = DEF_SPH_FACTOR;
   _axislen = DEF_AXIS_LEN;
   scaleFactor = 1.f;
   _skeleton = 0;
   _needsInit = true;
   _showBones = true;
   _showJoints = true;
 }

SkScene::~SkScene () = default;

static boost::intrusive_ptr<SrSnGroup> make_joint_group(const SkJoint* j, SkSkeleton* s, std::vector<boost::intrusive_ptr<SrSnGroup>>& _jgroup) {
	if (!j)
		return nullptr;
	int i;
	if (j->index() < 0) {
		SmartBody::util::log("Joint %s cannot be added to scene graph since joint index is %d", j->jointName().c_str(), j->index());
		return nullptr;
	}
	boost::intrusive_ptr<SrSnGroup> g(new SrSnGroup);
	g->separator(true);
	_jgroup[j->index()] = g;

	// insert children recursivelly
	for (i = 0; i < j->num_children(); i++) {
		auto group = make_joint_group(j->child(i), s, _jgroup);
		if (group)
			g->add(group);
	}

	for (auto & group : _jgroup) {
		if (group == nullptr)
			group.reset(new SrSnGroup()); // create dummy group for dangling joints
	}

	return g;
}

enum GroupPos { AxisPos=0, MatrixPos=1, SpherePos=2, GeoPos=3 };
enum GeoGroupPos { VisgeoPos=0, ColgeoPos=1, FirstCylPos=2 };

void SkScene::init ( boost::intrusive_ptr<SkSkeleton> s, float scale )
 {
   remove_all();
   _jgroup.clear();
   // unref seems to make things unstable with random crash. Since SkScene is only getting a pointer to the skeleton, I think its parent is responsible for cleaning it up. 
   //if ( _skeleton ) { _skeleton->unref(); _skeleton=0; }
      
   if ( !s ) return;
   _skeleton = s;

   _needsInit = true;
   scaleFactor = scale;
}

void SkScene::initInternal()
{
	_needsInit = false;
	if (!_skeleton)
		return;

   SrMat arot;
   SrSnGroup* gaxis;
   boost::intrusive_ptr<SrSnLines> axis;
   boost::intrusive_ptr<SrSnSphere> sphere;
   SrSnSharedModel* smodel;

   auto& joints = _skeleton->joints ();
   _jgroup.resize( joints.size(), nullptr );

   SkJoint* root = _skeleton->root();
   if (!root)
	   return;
//   const char* root_name = root->name().get_string();  // expose to debugger
   auto g = make_joint_group ( root, _skeleton.get(), _jgroup );
   if (!g)
   {
	   SmartBody::util::log("Skeleton %s cannot be added to the scene.", _skeleton->getName().c_str());
	   return;
   }
   g->separator ( true );
  
   float height = _skeleton->getCurrentHeight();
   scaleFactor =  height / 175.0f * 1.0f;
  // if (scaleFactor == 0.0f)
	//   scaleFactor = _scale;
   axis = new SrSnLines; // shared axis
   axis->shape().push_axis ( SrVec::null, _axislen*scaleFactor, 3, "xyz"/*let*/, false/*rule*/ );

   add ( g );

   for (size_t i=0; i<joints.size(); i++ )
    { 
      SkJoint* joint_p = joints[i].get();
      // axis shows the frame after correction, but before the joint local rotation:
      gaxis = new SrSnGroup;
      gaxis->separator ( true );
      gaxis->add ( new SrSnMatrix );
      gaxis->add ( axis.get() );
      gaxis->visible ( false );
      joint_p->quat()->prerot().get_mat(arot);
      arot.setl4 ( joints[i]->offset() );
      ((SrSnMatrix*)gaxis->get(0))->set ( arot );

      // add all visual elements:
	  auto& group_p = _jgroup[i];
      group_p->add ( gaxis, AxisPos );

	sphere = createSphere(scaleFactor*1.0f);	 
	group_p->add ( new SrSnMatrix, MatrixPos );
	group_p->add ( sphere.get(), SpherePos );
	//group_p->add (axis);
	if (!_showJoints)
		group_p->visible(false);


      g = new SrSnGroup; // geometry group of the joint
      group_p->add ( g, GeoPos );

      smodel = new SrSnSharedModel(joints[i]->visgeo()); // ok if visgeo is null
      g->add ( smodel ); // at VisgeoPos

      smodel = new SrSnSharedModel(joints[i]->colgeo());
      smodel->visible ( false );
      g->add ( smodel ); // at ColgeoPos
    
		for (size_t j=0; j < (size_t) joints[i]->num_children(); j++ )
		{ 
			SrSnCylinder* c = new SrSnCylinder;
			c->color(SrColor::gray);
			//c->color(SrColor::white);
			c->shape().a = SrPnt::null;
			c->shape().b = joints[i]->child(j)->offset();
			c->shape().radius = scaleFactor * _cradius;
			c->visible ( _showBones );
			g->add ( c ); // starting at FirstCylPos
		}


	  // end effector for bone mode
	  if (joints[i]->num_children() == 0)
	  {
		 SrSnSphere* sphere = new SrSnSphere;
		 sphere->color(SrColor::gray);
		 //sphere->color(SrColor::white);
		 sphere->shape().radius = scaleFactor * _cradius * _sfactor ;
		 sphere->visible ( true );
//		 sphere->ref();
         g->add ( sphere ); 	
	  }
    }
   //sphere->unref();
   update ();


 }

void SkScene::update ()
 {
   if ( !_skeleton ) return;
   if (_needsInit)
	   initInternal();
   auto& joints = _skeleton->joints ();
   for (size_t i=0; i<joints.size(); i++ ) update ( i );
 }

void SkScene::update ( int j )
 { 
   if ( !_skeleton ) return;
   SkJoint* joint = _skeleton->joints()[j].get();
   joint->update_lmat();
   if (_jgroup.size() <= j) 
	   return; // workaround for size mismatch between _jgroup and joints() size
   SrSnMatrix* mat =  ((SrSnMatrix*)_jgroup[j]->get(MatrixPos));
   if (mat)  mat->set ( joint->lmat() );
 }

void SkScene::rebuild ()
 {
   if ( !_skeleton ) return;
   auto& joints = _skeleton->joints();
   for (size_t i=0; i<joints.size(); i++ ) rebuild ( i );
 }

void SkScene::rebuild ( int j )
 { 
   if ( !_skeleton ) return;
   SrSnGroup* g;
   SkJoint* joint = _skeleton->joints()[j].get();
   joint->update_lmat();

   // update framerot:
   SrMat arot;
   joint->quat()->prerot().get_mat(arot);
   arot.setl4 ( joint->offset() );
   g = (SrSnGroup*)_jgroup[j]->get(AxisPos);
  ((SrSnMatrix*)g->get(0))->set ( arot );

   // update matrix:
   ((SrSnMatrix*)_jgroup[j]->get(MatrixPos))->set ( joint->lmat() );
   
   // update geometries:
   g = (SrSnGroup*)_jgroup[j]->get(GeoPos);
   ((SrSnSharedModel*)g->get(VisgeoPos))->changed(true);
   ((SrSnSharedModel*)g->get(ColgeoPos))->changed(true);

   // update links:
   int i;
   for ( i=0; i<joint->num_children(); i++ )
    { SrSnCylinder* c = (SrSnCylinder*)g->get(FirstCylPos+i);
      c->shape().b = joint->child(i)->offset();
    }
 }

void SkScene::set_visibility ( int skel, int visgeo, int colgeo, int vaxis )
 {
   if ( !_skeleton ) return;

   if (_needsInit)
	   initInternal();

   auto& joints = _skeleton->joints();
   for (const auto & joint : joints)
    { 
      set_visibility ( joint.get(), skel, visgeo, colgeo, vaxis );
    }
 }

void SkScene::set_visibility ( SkJoint* joint, int skel, int visgeo, int colgeo, int vaxis )
 {
   SrSnCylinder* cyl;
   SrSnSphere* sphere;
   SrSnGroup* gaxis;
   SrSnSharedModel* vismodel;
   SrSnSharedModel* colmodel;

   int i, j;
   
   i = joint->index();
   auto& g = _jgroup[i];

   if ( skel!=-1 )
    { sphere = (SrSnSphere*) g->get(SpherePos);
      sphere->visible ( skel? true:false );
    }

   if ( vaxis!=-1 )
    { gaxis = (SrSnGroup*) g->get(AxisPos);
      gaxis->visible ( vaxis? true:false );
    }

   auto group = (SrSnGroup*)_jgroup[i]->get(GeoPos); // the geometry group

   if ( skel!=-1 )
    { for ( j=FirstCylPos; j<group->num_children(); j++ )
       { cyl = (SrSnCylinder*) group->get(j);
         cyl->visible ( skel? true:false );
       }
    }

   if ( visgeo!=-1 )
    { vismodel = (SrSnSharedModel*) group->get(VisgeoPos);
      vismodel->visible ( visgeo? true:false );
    }

   if ( colgeo!=-1 )
    { colmodel = (SrSnSharedModel*) group->get(ColgeoPos);
      colmodel->visible ( colgeo? true:false );
    }
 }


void SkScene::setJointRadius( SkJoint* joint, float radius )
{
	int i;
	SrSnSphere* sphere;
	i = joint->index();
	_cradius = radius;
	if (_jgroup.size() > i)
	{
		auto& g = _jgroup[i];
		sphere = (SrSnSphere*) g->get(SpherePos);
		sphere->shape().radius = _cradius * _sfactor * scaleFactor;	
	}
	
}

void SkScene::setJointColor( SkJoint* joint, SrColor color )
{
	int i;
	SrSnSphere* sphere;
	i = joint->index();
	auto& g = _jgroup[i];
	sphere = (SrSnSphere*) g->get(SpherePos);
	sphere->color(color);
	sphere->override_material(sphere->material());
}

void SkScene::set_skeleton_radius ( float r )
 {
   SrSnCylinder* cyl;
   SrSnSphere* sphere;
   
   if ( _cradius==r ) return;
   _cradius = r;
   

   int i, j;
   for ( i=0; i<_jgroup.size(); i++ )
    { auto& g = _jgroup[i];
      sphere = (SrSnSphere*) g->get(SpherePos);
      sphere->shape().radius = _cradius * _sfactor;

      g = (SrSnGroup*)_jgroup[i]->get(GeoPos); // the geometry group

      for ( j=FirstCylPos; j<g->num_children(); j++ )
       { cyl = (SrSnCylinder*) g->get(j);
         cyl->shape().radius = _cradius;
       }
    }
 }

void SkScene::set_axis_length ( float l )
 {
   SrSnLines* axis;

   if ( _axislen==l || _skeleton==nullptr ) return;
   _axislen = l;

    auto& joints = _skeleton->joints();
   
   if (_jgroup.size() > 0)
    { auto& g = _jgroup[0];
      axis = (SrSnLines*) ((SrSnGroup*)g->get(AxisPos))->get(1);
      axis->shape().init();
      axis->shape().push_axis ( SrVec::null, _axislen, 3, "xyz"/*let*/, false/*rule*/ );
      // as the axis is shared, we can update only the first one
    }
 }

void SkScene::mark_geometry ( SkJoint* j )
 {
   SrSnGroup* g;
   SrSnSharedModel* model;
   SrMaterial mtl;
   mtl.diffuse = SrColor::red;

   g = (SrSnGroup*)_jgroup[j->index()]->get(GeoPos); // the geometry group

   int k;
   for ( k=FirstCylPos; k<g->num_children(); k++ )
     ((SrSnCylinder*)g->get(k))->override_material ( mtl );

   model = (SrSnSharedModel*) g->get(VisgeoPos);
   model->override_material ( mtl );

   model = (SrSnSharedModel*) g->get(ColgeoPos);
   model->override_material ( mtl );
 }

void SkScene::set_geometry_style ( SkJoint* j, SrModel* m, srRenderMode mode, bool mark, int alpha )
 {
   int geo;

   if ( !m ) return;
   else if ( j->visgeo()==m ) geo=VisgeoPos;
   else if ( j->colgeo()==m ) geo=ColgeoPos;
   else return;

   SrSnGroup* g;
   SrSnSharedModel* model;

   g = (SrSnGroup*)_jgroup[j->index()]->get(GeoPos); // the geometry group

   model = (SrSnSharedModel*) g->get(geo);

   if ( mode==srRenderModeDefault )
    model->restore_render_mode ();
   else
    model->override_render_mode ( mode );

   if ( alpha>=0 )
    { int i;
      SrModel& m = model->shape();
      for ( i=0; i<m.M.size(); i++ ) m.M[i].diffuse.a = alpha;
    }

   if ( mark )
    { SrMaterial mtl;
      mtl.diffuse = SrColor::red;
      if ( alpha>=0 ) mtl.diffuse.a = alpha;
      model->override_material ( mtl );
    }
   else
    model->restore_material ();
 }

void SkScene::unmark_all_geometries ()
 {
   SrSnGroup* g;
   SrSnSharedModel* model;

   int i, k;
   for ( i=0; i<_jgroup.size(); i++ )
    { g = (SrSnGroup*)_jgroup[i]->get(GeoPos); // the geometry group

      for ( k=FirstCylPos; k<g->num_children(); k++ )
        ((SrSnCylinder*)g->get(k))->restore_material();

      model = (SrSnSharedModel*) g->get(VisgeoPos);
      model->restore_material ();

      model = (SrSnSharedModel*) g->get(ColgeoPos);
      model->restore_material ();
    }
 }

//============================= static ===================================

void SkScene::get_defaults ( float& sradius, float& alen )
 {
   sradius = _cradius;
   alen = _axislen;
 }

boost::intrusive_ptr<SrSnSphere> SkScene::createSphere(float scaleFactor )
{
	boost::intrusive_ptr<SrSnSphere> sphere(new SrSnSphere); // shared sphere
	sphere->color(SrColor::white);   
	sphere->shape().radius = scaleFactor * _cradius * _sfactor;
	sphere->visible ( true );
	//sphere->ref();	
	return sphere;
}

void SkScene::setShowJoints(bool val)
{
	_showJoints = val;
}

void SkScene::setShowBones(bool val)
{
	_showBones = val;
}

//============================= EOF ===================================
