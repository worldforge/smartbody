/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/



#include <iostream>
#include <sstream>
#include <string>

#include "bml_target.hpp"
#include "sbm/BMLDefs.h"
#include <sb/SBScene.h>
#include <sb/SBUtilities.h>

#define DEBUG_BML_TARGET				(0)


using namespace std;
//using namespace BML;
using namespace xml_utils;

/**
 *  Parse joint attribute string into a valid SkJoint*.
 *
 *  Eventually this parsing system will need to support events for lookup query delays.
 */

SkJoint* BML::parse_target( const XMLCh* Attr, DOMElement* Elem, SmartBody::SBScene* scene ) {

	return( const_cast<SkJoint*>( parse_target( Elem->getTagName(), Elem->getAttribute( Attr ), scene ) ) );
}

const SkJoint* BML::parse_target( const XMLCh* tagname, const XMLCh* attrTarget, SmartBody::SBScene* scene ) {
	// TODO: If the first non-whitespace character is 0..9.-+, then assume it is a coordinate
	XMLStringTokenizer tokenizer( attrTarget );	
	std::stringstream strstr;
	switch( tokenizer.countTokens() ) {
		case 1: {
			// One token is an object id
			string object_id = xml_utils::asciiString(tokenizer.nextToken());
			string bone_id;
			SbmPawn* target;


			// TODO: Revisit the target syntax.
			// Currently, we use "object_id:bone_id", but this is probably not sufficient
			string::size_type colon_index = object_id.find( ':' );
			if( colon_index == string::npos ) {
				// Missing ':' object/bone delimiter, so guess...
				target = SmartBody::SBScene::getScene()->getCharacter( object_id );
				if( target ) {
					// Target is a character, look at eyeball
					bone_id = "eyeball_left";
#if DEBUG_BML_TARGET
					{
						std::stringstream strstr;
						strstr << "DEBUG: BML::parse_target(): Gaze: Found target character \"" << object_id << "\". Assuming joint \""<<bone_id<<"\".";
						SmartBody::util::log(strstr.str().c_str());
					}
#endif
        } else {
					// Target is a pawn, look at world offset
					target =  SmartBody::SBScene::getScene()->getPawn( object_id );
					if( target ) {
						bone_id = SbmPawn::WORLD_OFFSET_JOINT_NAME;
#if DEBUG_BML_TARGET
						{
							std::stringstream strstr;
							strstr << "DEBUG: BML::parse_target(): Gaze: Found target pawn \"" << object_id << "\". Assuming joint \""<<bone_id<<"\".";
							SmartBody::util::log(strstr.str().c_str());
						}
#endif
					} else {
						// TODO: Query World State Protocol (requires event delay)
						std::stringstream strstr;
						strstr << "WARNING: BML::parse_target(): Gaze: Unknown target \""<<object_id<<"\". (TODO: Query WSP.) Behavior ignored."<< endl;
						SmartBody::util::log(strstr.str().c_str());
						return NULL;
					}
				}
			} else {
				// Found ':' object/bone delimiter
				bone_id = object_id.substr( colon_index+1 );
				object_id.erase( colon_index );
#if DEBUG_BML_TARGET
					cout << "DEBUG: BML::parse_target(): Gaze:\tobject_id \""<<object_id<<"\",\tbone_id \""<<bone_id<<"\"." <<endl;
#endif
				target =  SmartBody::SBScene::getScene()->getPawn( object_id );
				if( target==NULL ) {

					// we've failed to find object:bone locally, now query wsp
					target =  SmartBody::SBScene::getScene()->getPawn( object_id + ":" + bone_id );
					if( target )
					{
						bone_id = SbmPawn::WORLD_OFFSET_JOINT_NAME;
#if DEBUG_BML_TARGET
						{
							std::stringstream strstr;
							strstr << "DEBUG: BML::parse_target(): Gaze: Found target pawn \"" << object_id << "\". Assuming joint \""<<bone_id<<"\".";
							SmartBody::util::log(strstr.str().c_str());
						}
#endif
					}
					else
					{
						std::stringstream strstr;
						strstr << "WARNING: BML::parse_target(): Gaze: Unknown object id \""<<object_id<<"\". (TODO: Query WSP.) Behavior ignored."<< endl;
						SmartBody::util::log(strstr.str().c_str());
						return NULL;
					}
				}
			}

			// Look up the joint
			const SkJoint* joint = target->get_joint( bone_id.c_str() );
			if( joint == NULL ) {				
				if (SmartBody::SBScene::getScene()->getCharacter( object_id )) // target is a character but does not have eye joint
				{
					return target->get_world_offset_joint();
				}
				strstr << "WARNING: BML::parse_target(): Gaze: Target \""<<object_id<<"\" does not have joint \""<<bone_id<<"\". Behavior ignored.";
				SmartBody::util::log(strstr.str().c_str());
				return NULL;
			}

			return joint;
		}
		case 3: {
			// Three tokens is a global position
			XMLCh* token = tokenizer.nextToken();

			// TODO
			strstr << "WARNING: BML::parse_target(): Unimplented <"<<tagname<<" "<< BMLDefs::ATTR_TARGET<<"=\"x, y, z\" ... />.  Behavior ignored.";
			SmartBody::util::log(strstr.str().c_str());
			return NULL;
		}
		default: {
			strstr << "WARNING: BML::parse_target(): Invalid token count in <"<<tagname<<" "<< BMLDefs::ATTR_TARGET<<"=\"x, y, z\" ... />.  Behavior ignored.";
			SmartBody::util::log(strstr.str().c_str());
			return NULL;
		}
	}  // end switch( tokenizer.countTokens() )
}

const SbmPawn* BML::parse_target_pawn( const XMLCh* tagname, const XMLCh* attrTarget, SmartBody::SBScene* scene )
{
	// TODO: If the first non-whitespace character is 0..9.-+, then assume it is a coordinate
	XMLStringTokenizer tokenizer( attrTarget );	
	std::stringstream strstr;
	// One token is an object id
	string object_id = xml_utils::asciiString(tokenizer.nextToken());
	string bone_id;
	SbmPawn* target;

	// TODO: Revisit the target syntax.
	// Currently, we use "object_id:bone_id", but this is probably not sufficient
	// Target is a pawn, look at world offset
	target =  SmartBody::SBScene::getScene()->getPawn( object_id );
	if( target ) 
	{
		return target;	   
	}			
	return NULL;
}
