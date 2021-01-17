/*  sr_sn_group.h - part of Motion Engine and SmartBody-lib
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
 *  License along with SmarBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Marcelo Kallmann, USC (currently UC Merced)
 */

# ifndef SR_SN_GROUP_H
# define SR_SN_GROUP_H

/** \file sr_sn_group.h 
 * groups scene nodes
 */

#include "SBABI.h"
#include "sr_sn.h"
#include "sr_shared_ptr.hpp"
//======================================= SrSnGroup ====================================

/*! \class SrSnGroup sr_scene.h
    \brief groups scene graph nodes

    SrSnGroup keeps a list of children. The group can be set or not
    to behave as a separator of the render state during action traversals.
    By default, it is not set as a separator. */   
class SBAPI SrSnGroup : public SrSn
 { private :
    char _separator;
    std::vector<boost::intrusive_ptr<SrSn>> _children;

   public :
    static const char* class_name;

   public :
    /*! The destructor will remove all children in the subtree.
        Only accessible through unref(). */
    ~SrSnGroup () override;

   public :
    /*! Default constructor. Separator behavior is set to false. */
    SrSnGroup ();

    /*! Sets the group separator behavior. If it is set to true, the
        render state is pushed to restored after the traversal of the
        group children. Mainly used to localize the effect of transformation
        matrices, applying only to the group children. */
    void separator ( bool b ) { _separator = (char)b; }

    /*! Returns the group separator behavior state. */
    bool separator () const { return _separator != 0; }

    /*! Changes the capacity of the children array. If the requested capacity
        is smaller than the current size, nothing is done. */
    void capacity ( int c );

    /*! Compresses the children array. */
    void compress () { _children.shrink_to_fit(); }

    /*! Returns the number of children. */
    size_t size () const { return _children.size(); }

    /*! Returns the number of children (same as size()). */
	size_t num_children () const { return _children.size(); }

    /*! Get the child at position pos. If pos is invalid (as -1) the last
        child is returned, and if there are no children, 0 is returned. */
    SrSn* get ( int pos ) const;

    /*! Searches for the child position. -1 is returned if the child is 
        not found. */
    int search ( SrSn *n ) const;

    /*! If pos<0 or pos>=num_children(), sn is appended. Otherwise, sn is 
        inserted in the given position in the children array, and
        reallocation is done if required. */
    void add ( boost::intrusive_ptr<SrSn> sn, int pos=-1 );

    /*! Removes one child.
        If the node removed is no more referenced by the scene graph, it is 
        deallocated together with all its sub-graph and 0 is returned. 0 is 
        also returned if the group has no children. If pos==-1 (the default)
        or pos is larger than the maximum child index, the last child is 
        removed. Otherwise, the removed node is returned. */
	boost::intrusive_ptr<SrSn> remove ( int pos=-1 );

    /*! Searches for the position of the given child pointer and removes it 
        with remove_child ( position ). */
	boost::intrusive_ptr<SrSn> remove ( SrSn *n );

    /*! Removes child pos and insert sn in place. Same reference rules of 
        remove applies. Will return the old node or 0 if the node is deleted
        because its ref counter reached zero. If pos is out of range, 0 is 
        returned and nothing is done. */
	boost::intrusive_ptr<SrSn> replace ( int pos, SrSn *sn );

    /*! Removes all children, calling the unref() method of each children. 
        The result is the same as calling remove_child() for each child. */
    void remove_all ();
 };

//================================ End of File =================================================

# endif  // SR_SN_GROUP_H

