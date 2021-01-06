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

#include "SBEvent.h"
#include <sb/SBScene.h>
#include <sb/SBCommandManager.h>
#include <sb/SBScene.h>
#include <sb/SBSceneListener.h>
#include <sb/SBAttribute.h>

#include <boost/algorithm/string/replace.hpp>

namespace SmartBody {

SBEventHandler::SBEventHandler() : SBObject()
{
	m_type = "";
	m_action = "";
	_enabled = true;
	this->createBoolAttribute("enable", true, true, "Event", 10, false, false, false, "Whether this event is enabled.");
}

void SBEventHandler::setEnable(bool val)
{
	_enabled = val;
	BoolAttribute* boolAttribute = dynamic_cast<BoolAttribute*>(this->getAttribute("enable"));
	boolAttribute->setValueFast(val);
}

bool SBEventHandler::isEnable()
{
	return _enabled;
}


void SBEventHandler::notify(SBSubject* subject)
{
	SBObject::notify(subject);

	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "enable")
		{
			BoolAttribute* boolAttr = dynamic_cast<BoolAttribute*>(attribute);
			if (boolAttr)
			{
				bool val = boolAttr->getValue();
				this->setEnable(val);
			}
		}
	}
}


SBBasicHandler::SBBasicHandler() : SBEventHandler() 
{
	this->createStringAttribute("eventName", "", true, "Event", 20, false, false, false, "Name of the event that this handler will respond to.");
	this->createStringAttribute("eventAction", "", true, "Event", 30, false, false, false, "Behavior of event when triggered (command run using Python).");
}

void SBBasicHandler::setAction(const std::string& action)
{ 
	m_action = action;
	StringAttribute* stringAttr = dynamic_cast<StringAttribute*>(this->getAttribute("eventAction"));
	stringAttr->setValueFast(action);
}

const std::string& SBBasicHandler::getAction()
{ 
	return m_action;
}

void SBBasicHandler::executeAction(SBEvent* event) 
{
	/*
	std::string action = getAction();
	boost::replace_all(action, "$1", event->getParameters());

	
	SmartBody::SBScene::getScene()->getCommandManager()->execute((char*) action.c_str());
	*/
	StringAttribute* stringAttr = dynamic_cast<StringAttribute*>(this->getAttribute("eventAction"));
	SmartBody::SBScene::getScene()->run(stringAttr->getValue());
}

void SBBasicHandler::notify(SBSubject* subject)
{
	SBEventHandler::notify(subject);

	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		if (attribute->getName() == "eventAction")
		{
			StringAttribute* stringAttr = dynamic_cast<StringAttribute*>(attribute);
			if (stringAttr)
			{
				setAction(stringAttr->getValue());
			}
		}
	}
}



SBEventManager::SBEventManager(SBScene& scene): SBSceneOwned(scene) { }

SBEventManager::~SBEventManager()
{
	for (auto & eventHandler : eventHandlers)
	{
		SBEventHandler* handler = eventHandler.second;
//		delete handler;
	}
}

void SBEventManager::handleEvent(SBEvent* e)
{
	std::string type = e->getType();
	std::string params = e->getParameters();

	std::vector<SBSceneListener*>& listeners = _scene.getSceneListeners();
	for (auto & listener : listeners)
	{
		listener->OnEvent(type, params);
	}
	
	// find the appropriate event handler

	auto iter = eventHandlers.find(e->getType());
	if (iter == eventHandlers.end())
		return;

	SBEventHandler* handler = (*iter).second;
	if (handler->isEnable())
		handler->executeAction(e);	
}

void SBEventManager::handleEventRemove(SBEvent* e)
{
	handleEvent(e);
	delete e;
}

SBEvent* SBEventManager::createEvent(const std::string& type, const std::string parameters, const std::string source)
{
	SBEvent* event = new SBEvent();
	event->setType(type);
	event->setParameters(parameters);
	event->setSource(source);

	return event;
}

void SBEventManager::addEventHandler(const std::string& type, SBEventHandler* handler)
{
	removeEventHandler(type);

	handler->setName(type);

	eventHandlers.insert(std::pair<std::string, SBEventHandler*>(type, handler));
}

void SBEventManager::removeEventHandler(const std::string& type)
{
	auto iter = eventHandlers.find(type);
	if (iter != eventHandlers.end())
	{
		SBEventHandler* oldHandler = (*iter).second;
		eventHandlers.erase(iter);
#if !defined(SB_NO_JAVASCRIPT) && defined(EMSCRIPTEN)
		delete oldHandler; // deleting old handler causes crash when handler is created with Python - need to fix this
#endif
	}
}

int SBEventManager::getNumEventHandlers()
{
	return eventHandlers.size();
}

SBEventHandler* SBEventManager::getEventHandlerByIndex(int num)
{
	int counter = 0;
	for (auto & eventHandler : eventHandlers)
	{
		if (counter == num)
		{
			SBEventHandler* handler = eventHandler.second;
			return handler;
		}
	}

	return nullptr;
}

SBEventHandler* SBEventManager::getEventHandler(const std::string& type)
{
	auto iter = eventHandlers.find(type);
	if (iter != eventHandlers.end())
	{
		SBEventHandler* handler = (*iter).second;
		return handler;
	}

	return nullptr;
}

std::map<std::string, SBEventHandler*>& SBEventManager::getEventHandlers()
{
	return eventHandlers;
}


}
