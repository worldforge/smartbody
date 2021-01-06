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

#include "SBAnimationStateManager.h"

#include <sb/SBAnimationState.h>
#include <sb/SBMotionBlendBase.h>
#include <sb/SBAnimationTransition.h>
#include <sb/SBCharacter.h>
#include <sb/SBScene.h>
#include <sb/SBSceneListener.h>
#include "SBUtilities.h"
#include <controllers/me_ct_param_animation.h>

namespace SmartBody {

SBAnimationBlendManager::SBAnimationBlendManager(SBScene& scene) : SBSceneOwned(scene)
{
}

SBAnimationBlendManager::~SBAnimationBlendManager() = default;

std::vector<std::string> SBAnimationBlendManager::getAutoBlendTransitions( const std::string& characterName, const std::string& targetBlend )
{
	std::vector<std::string> pathVec;
	

	return pathVec;		
}

bool SBAnimationBlendManager::addBlendToGraph( const std::string& name )
{
	BoostGraph::vertex_descriptor v = stateGraph.vertex(name);
	if (v == BoostGraph::null_vertex()) // the state does not exist in the graph
	{		
		stateGraph.add_vertex(name);	
		//boost::put(boost::get(boost::vertex_name_t,stateGraph),stateGraph,name,name);
		//boost::put(boost::vertex_name_t,)
		
	}
	return true;
}

bool SBAnimationBlendManager::addTransitionEdgeToGraph( const std::string& source, const std::string& dest )
{
	BoostGraph::vertex_descriptor vs = stateGraph.vertex(source), vd = stateGraph.vertex(dest);
	
	if (vs == BoostGraph::null_vertex() || vd == BoostGraph::null_vertex()) return false;
	std::pair<BoostGraph::edge_descriptor,bool> transitionEdge = boost::edge_by_label(source,dest,stateGraph);
	// edge already exist
	if (transitionEdge.second) return false;

	// otherwise add transition edge
	boost::add_edge_by_label(source,dest,stateGraph); return true;
}

SBAnimationBlend0D* SBAnimationBlendManager::createBlend0D(const std::string& name)
{

	SBAnimationBlend0D* blend = new SBAnimationBlend0D(name);

	addBlendToGraph(name);
	_blends.emplace_back(blend);

	std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnObjectCreate(blend);
	}
	return blend;
}

SBAnimationBlend1D* SBAnimationBlendManager::createBlend1D(const std::string& name)
{

	SBAnimationBlend1D* blend = new SBAnimationBlend1D(name);
	
	addBlendToGraph(name);
	_blends.emplace_back(blend);
	
	std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnObjectCreate(blend);
	}
	return blend;
}

SBAnimationBlend2D* SBAnimationBlendManager::createBlend2D(const std::string& name)
{

	SBAnimationBlend2D* blend = new SBAnimationBlend2D(name);
	
	addBlendToGraph(name);
	_blends.emplace_back(blend);

	std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnObjectCreate(blend);
	}

	return blend;
}

SBAnimationBlend3D* SBAnimationBlendManager::createBlend3D(const std::string& name)
{

	SBAnimationBlend3D* blend = new SBAnimationBlend3D(name);
	
	addBlendToGraph(name);
	_blends.emplace_back(blend);

	std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnObjectCreate(blend);
	}
	return blend;
}

SBMotionBlendBase* SBAnimationBlendManager::createMotionBlendBase( const std::string& name, const std::string& skelName, int dimension )
{
	SBMotionBlendBase* blend = new SBMotionBlendBase(_scene, name,skelName, dimension);
	
	//	addBlendToGraph(name);
	_blends.emplace_back(blend);

	std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnObjectCreate(blend);
	}
	return blend;
}

SBAnimationTransition* SBAnimationBlendManager::createTransition(const std::string& source, const std::string& dest)
{	
	
	
	SBAnimationBlend* sourceBlend = getBlend(source);
	if (!sourceBlend)
	{
		SmartBody::util::log("Source state %s does not exist. No transition created.", source.c_str());
		return nullptr;
	}
	SBAnimationBlend* destBlend = getBlend(dest);
	if (!destBlend)
	{
		SmartBody::util::log("Destination state %s does not exist. No transition created.", dest.c_str());
		return nullptr;
	}
	SBAnimationTransition* transition = new SBAnimationTransition(source + "/" + dest);
	transition->set(sourceBlend, destBlend);

	_transitions.emplace_back(transition);

	std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnObjectCreate(transition);
	}
	return transition;
}

SBAnimationBlend* SBAnimationBlendManager::getBlend(const std::string& name)
{
	for (auto & _blend : _blends)
	{
		if (_blend->stateName == name)
			return _blend;
	}
	return nullptr;
}

int SBAnimationBlendManager::getNumBlends()
{
	return _blends.size();
}

std::vector<std::string> SBAnimationBlendManager::getBlendNames()
{
	std::vector<std::string> states;
	for (auto & _blend : _blends)
	{
		states.emplace_back(_blend->stateName);
	}
	return states;
}

std::vector<std::string> SBAnimationBlendManager::getTransitionBlends(const std::string& source)
{
	std::vector<std::string> blends;
	
	for (auto & _transition : _transitions)
	{
		if (_transition->getSourceBlend()->stateName == source)
			blends.emplace_back(_transition->getDestinationBlend()->stateName);
	}

	return blends;
}

SBAnimationTransition* SBAnimationBlendManager::getTransition(const std::string& source, const std::string& dest)
{
	for (auto & _transition : _transitions)
	{
		if (_transition->getSourceBlend()->stateName == source &&
			_transition->getDestinationBlend()->stateName == dest)
			return _transition;
	}

	return nullptr;
}

SBAnimationTransition* SBAnimationBlendManager::getTransitionByName( const std::string& transitionName )
{
	for (auto & _transition : _transitions)
	{
		if (_transition->getTransitionName() == transitionName)
			return _transition;
	}

	return nullptr;
}

SBAnimationTransition* SBAnimationBlendManager::getTransitionByIndex(int id)
{
	if (id >= 0 && id < (int) _transitions.size())
	{
		SBAnimationTransition* animTransition = _transitions[id];
		return animTransition;
	}
	
	return nullptr;
}


int SBAnimationBlendManager::getNumTransitions()
{
	return _transitions.size();
}

std::vector<std::string> SBAnimationBlendManager::getTransitionNames()
{
	std::vector<string> transitionNames;
	for (auto & _transition : _transitions)
	{
		transitionNames.emplace_back( _transition->getTransitionName() );
	}
	return transitionNames;
}

std::string SBAnimationBlendManager::getCurrentBlend(const std::string& characterName)
{
	SBCharacter* character = _scene.getCharacter(characterName);
	if (!character)
		return "";

	if (!character->param_animation_ct)
		return "";

	PABlendData* blendData = character->param_animation_ct->getCurrentPABlendData();
	if (!blendData)
		return "";
	return blendData->state->stateName;
}

SrVec SBAnimationBlendManager::getCurrentBlendParameters(const std::string& characterName)
{
	SrVec params;
	SBCharacter* character = _scene.getCharacter(characterName);
	if (!character)
		return params;

	if (!character->param_animation_ct)
		return params;

	
	PABlendData* blendData = character->param_animation_ct->getCurrentPABlendData();
	if (!blendData)
		return params;

	SmartBody::SBAnimationBlend0D* state0D = dynamic_cast<SmartBody::SBAnimationBlend0D*>(blendData->state);
	if (state0D)
		return params;

	SmartBody::SBAnimationBlend1D* state1D = dynamic_cast<SmartBody::SBAnimationBlend1D*>(blendData->state);
	SmartBody::SBAnimationBlend2D* state2D = dynamic_cast<SmartBody::SBAnimationBlend2D*>(blendData->state);
	SmartBody::SBAnimationBlend3D* state3D = dynamic_cast<SmartBody::SBAnimationBlend3D*>(blendData->state);
	SmartBody::SBMotionBlendBase* blendBase = dynamic_cast<SmartBody::SBMotionBlendBase*>(blendData->state);
	if (state1D)
	{
		blendData->state->getParametersFromWeights(params[0], blendData->weights);
		return params;
	}
	if (state2D)
	{
		blendData->state->getParametersFromWeights(params[0], params[1], blendData->weights);
		return params;
	}
	if (state3D || blendBase)
	{
		blendData->state->getParametersFromWeights(params[0], params[1], params[2], blendData->weights);
		return params;
	}
	return params;
}

bool SBAnimationBlendManager::isBlendScheduled(const std::string& characterName, const std::string& stateName)
{
	SBCharacter* character = _scene.getCharacter(characterName);
	if (!character)
		return false;

	if (!character->param_animation_ct)
		return false;

	return character->param_animation_ct->hasPABlend(stateName);
}

void SBAnimationBlendManager::removeAllBlends()
{
	// remove the transitions, too
	removeAllTransitions();

	for (auto & _blend : _blends)
	{
		delete _blend;
	}
	_blends.clear();

	stateGraph = BoostGraph();

}

void SBAnimationBlendManager::removeAllTransitions()
{
	for (auto & _transition : _transitions)
	{
		delete _transition;
	}
	_transitions.clear();
}

}