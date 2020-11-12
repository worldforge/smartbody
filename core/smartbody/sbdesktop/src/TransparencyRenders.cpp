#include "TransparencyRenders.h"


#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include "TransparentViewer.h"
#include <sbm/GPU/SbmShader.h>
#include <sbm/GPU/SbmShader.h>
#include <sbm/sbm_deformable_mesh.h>
#include <sbm/GPU/SbmDeformableMeshGPU.h>
#include <sr/sr_gl_render_funcs.h>

TransparencyRenders::TransparencyRenders(TransparentViewer* viewer)
{
	gridColor[0] = 0.5f;
   gridColor[1] = 0.5f;
   gridColor[2] = 0.5f;
   gridColor[3] = 1.0f;
   gridHighlightColor[0] = .3f;
   gridHighlightColor[1] = .3f;
   gridHighlightColor[2] = .3f;
   gridSize = 200.f;
   gridStep = 20.f;
//   gridSize = 400.0;
//   gridStep = 50.0;
   gridList = -1;
   sceneScale = .01f;
   _viewer = viewer;
}

TransparencyRenders::~TransparencyRenders()
{
}

void TransparencyRenders::drawGrid()
{
	//if (_data.gridMode == ModeNoGrid)
	//	return;
	if( gridList != -1 )	{
		glCallList( gridList );
		return;
	}

	GLfloat floor_height = 0.0f;

	glPushAttrib(GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
	bool colorChanged = false;
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

	glColor4f(gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glLineWidth(1);
//	glLineStipple(1, 0xAAAA);
	glBegin(GL_LINES);
	//float sceneScale =  SmartBody::SBScene::getScene()->getScale();
	float adjustedGridStep = gridStep;
	if (sceneScale > 0.f)
	{
		adjustedGridStep *= adjustedGridStep / .01f;
	}

	for (float x = -gridSize; x <= gridSize; x += adjustedGridStep)
	{
		if (x == 0.0) {
			colorChanged = true;
			glColor4f(gridHighlightColor[0], gridHighlightColor[1], gridHighlightColor[2], 1.0f);
		}
		glVertex3f(x, floor_height, -gridSize);
		glVertex3f(x, floor_height, gridSize);
		
		if (colorChanged) {
			colorChanged = false;
			glColor4f(gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
		}

	}
	for (float x = -gridSize; x <= gridSize; x += gridStep)
	{
		if (x == 0) {
			colorChanged = true;
			glColor4f(gridHighlightColor[0], gridHighlightColor[1], gridHighlightColor[2], 1.0f );
		}
		glVertex3f(-gridSize, floor_height, x);
		glVertex3f(gridSize, floor_height, x);
		if (colorChanged) {
			colorChanged = false;
			glColor4f(gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
		}
	}

	glEnd();
	glDisable(GL_BLEND);
//	glDisable(GL_LINE_STIPPLE);

	glPopAttrib();
}

void TransparencyRenders::initGridList()
{
	glDeleteLists(gridList, 1);
	gridList = glGenLists(1);
	if ( gridList == GL_INVALID_VALUE || gridList == GL_INVALID_OPERATION)
		return;
	glNewList(gridList, GL_COMPILE);
	drawGrid();
	glEndList();
}

void TransparencyRenders::drawCharacters(bool shadowPass)
{
	//_viewer->_data.render_action.apply (_viewer->_data.root );
	
	// simple drawing of characters as dots
	glColor3f(1.0f,.0f,.0f);   
	SmartBody::SBScene* scene = SmartBody::SBScene::getScene();
	const std::vector<std::string>& names = scene->getCharacterNames();
	for (std::vector<std::string>::const_iterator iter = names.begin();
		 iter != names.end();
		 iter++)
	{
		SmartBody::SBCharacter* cur =  scene->getCharacter(*iter);
		if(cur->dMeshInstance_p)
		{
			cur->dMeshInstance_p->setVisibility(true);
			cur->dMeshInstance_p->update();
			cur->scene_p->set_visibility(0,0,0,0);
			std::string display = cur->getStringAttribute("displayType");
			if (display == "GPUmesh")
			{
				cur->dMeshInstance_p->setVisibility(false);
			}
			else if (display == "mesh")
			{
				DeformableMeshInstance* meshInstance = cur->getActiveMesh();
				if(meshInstance)
				{
					//pawn->dMesh_p->update();
					if (!meshInstance->isStaticMesh()) // is skinned mesh
					{
		#if 0
						bool useBlendShape = false;
						SmartBody::SBCharacter* character = dynamic_cast<SmartBody::SBCharacter*> (pawn);
						if (character)
						{
							useBlendShape = character->getBoolAttribute("blendshape");
						}
						if (useBlendShape && meshInstance->getDeformableMesh())
						{
							//character->dMesh_p->blendShapes();
							meshInstance->getDeformableMesh()->blendShapes();
							//character->dMeshInstance_p->setDeformableMesh(character->dMesh_p);
						}
		#endif
						meshInstance->update();
						//meshInstance->updateFast();
						if ( (!SbmDeformableMeshGPU::useGPUDeformableMesh && meshInstance->getVisibility() == 1) || meshInstance->getVisibility() == 2)
						{
							bool showSkinWeight = (meshInstance->getVisibility() == 2);
							SrGlRenderFuncs::renderDeformableMesh(meshInstance, showSkinWeight);
							//				if (pawn->scene_p)
							//				pawn->scene_p->set_visibility(0,1,0,0);
							//_data->render_action.apply(character->scene_p);
						}
					}
					else
					{
						//meshInstance->blendShapes();
						meshInstance->blendShapeStaticMesh();

						// simply draw the static mesh
						SrGlRenderFuncs::renderDeformableMesh(meshInstance, false);
					}
				}
			}
			cur->dMeshInstance_p->setVisibility(true);
		}
	}

	bool hasGPUSupport = SbmShaderManager::getShaderSupport() != SbmShaderManager::NO_GPU_SUPPORT;
	if (hasGPUSupport)
    {
       // std::string shaderName = _viewer->_data.shadowmode == true && !shadowPass ? "Basic" : "BasicShadow";
        std::string shaderName = "Basic";
	    SbmShaderProgram* basicShader = SbmShaderManager::singleton().getShader(shaderName);
	    GLuint program = basicShader->getShaderProgram();
	    if (!shadowPass)
		    glUseProgram(program);		
	    GLuint useShadowMapLoc = glGetUniformLocation(program,"useShadowMap");
	    if (_viewer->_data.shadowmode == true && !shadowPass) // attach the texture
	    {		
    		
		    glActiveTexture(GL_TEXTURE7);
		    glBindTexture(GL_TEXTURE_2D, _viewer->_data.depthMapID);
		    //glMatrixMode(GL_TEXTURE);
		    //glLoadMatrixf(shadowTexMatrix.pt(0));
		    glCullFace(GL_BACK);
		    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		    glUniform1i(glGetUniformLocation(program, "tex"), 7); 		
		    glMatrixMode(GL_MODELVIEW);			
		    glUniform1i(useShadowMapLoc,1);		
	    }
	    else
	    {
		    glUniform1i(useShadowMapLoc,0);	
	    }
    }

	if( _viewer->_data.root )	{		
		_viewer->_data.render_action.apply ( _viewer->_data.root );
	}	

	
	/* draw the skeleton
	for (std::vector<std::string>::const_iterator iter = names.begin();
		 iter != names.end();
		 iter++)
	{
		glBegin(GL_LINES);     
		SmartBody::SBCharacter* cur =  scene->getCharacter(*iter);
		SmartBody::SBSkeleton* skeleton = cur->getSkeleton();
		int numJoints = skeleton->getNumJoints();
		for (int j = 0; j < numJoints; j++)
		{
			SmartBody::SBJoint* joint = skeleton->getJoint(j);
			SmartBody::SBJoint* parent = joint->getParent();
			if (parent)
			{
				const SrMat& gmat = joint->gmat();
				glVertex3f(gmat[12], gmat[13], gmat[14]);  
				const SrMat& pgmat = parent->gmat();
				glVertex3f(pgmat[12], pgmat[13], pgmat[14]);  
			}
			
		}
		glEnd();
		
	}
	*/
	
	
}
