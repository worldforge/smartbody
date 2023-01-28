#include "BMLObject.h"
#include <sstream>
#include <sb/SBAttribute.h>
#include "SBUtilities.h"
#include <boost/lexical_cast.hpp>

BMLObject::BMLObject()
{
	createStringAttribute("id", "", true, "Basic", 1000, false, false, false, 
		"Id of this BML request");
}

BMLObject::~BMLObject() = default;

BMLObject* BMLObject::copy()
{
	return new BMLObject();
}

void BMLObject::constructBML()
{
	std::stringstream strstr;

	strstr << "<" << getName();

	int numUsedElements = 0;
	auto& attributes = getAttributeList();
	for (auto & iter : attributes)
	{
		auto attribute = iter.second.get();

		auto* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
		if (boolAttribute)
		{
			if (boolAttribute->getValue() != boolAttribute->getDefaultValue())
			{
				strstr << " " << boolAttribute->getName() << "=\"" << (boolAttribute->getValue()? "true" : "false") << "\"";
				numUsedElements++;
			}
		}
		
		auto* intAttribute = dynamic_cast<SmartBody::IntAttribute*>(attribute);
		if (intAttribute)
		{
			if (intAttribute->getValue() != intAttribute->getDefaultValue())
			{
				strstr << " " << intAttribute->getName() << "=\"" << intAttribute->getValue() << "\"";
				numUsedElements++;
			}
		}

		auto* doubleAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
		if (doubleAttribute)
		{
			if (doubleAttribute->getValue() != doubleAttribute->getDefaultValue())
			{
				strstr << " " << doubleAttribute->getName() << "=\"" << doubleAttribute->getValue()<< "\"";
				numUsedElements++;
			}
		}

		auto* vecAttribute = dynamic_cast<SmartBody::Vec3Attribute*>(attribute);
		if (vecAttribute)
		{
			if (vecAttribute->getValue() != vecAttribute->getDefaultValue())
			{
				strstr << " " << vecAttribute->getName() << "=\"" << vecAttribute->getValue()[0] << " " << vecAttribute->getValue()[1] << " " << vecAttribute->getValue()[2] << "\"";
				numUsedElements++;
			}
		}

		auto* stringAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
		if (stringAttribute)
		{
			if (stringAttribute->getValue() != stringAttribute->getDefaultValue())
			{
				strstr << " " << stringAttribute->getName() << "=\"" << stringAttribute->getValue()<< "\"";
				numUsedElements++;
			}
		}
	}

	strstr << "/>";
	if (numUsedElements > 0)
		_bml = strstr.str();
	else
		_bml = "";
}

std::string BMLObject::getBML()
{
	return _bml;
}

void BMLObject::notify(SBSubject* subject)
{
	auto* attribute = dynamic_cast<SmartBody::SBAttribute*>(subject);
	if (attribute)
	{
		constructBML();
		notifyObservers();
	}
}

void BMLObject::parse(rapidxml::xml_node<>* node)
{
	std::string behaviorName = node->name();

	for (rapidxml::xml_attribute<>* attr = node->first_attribute();
		 attr; 
		 attr = attr->next_attribute())
	{
		std::string attrName = attr->name();
		std::string attrValue = attr->value();

		// check to see if the attribute exists on this object
		SmartBody::SBAttribute* attribute = this->getAttribute(attrName);
		if (!attribute)
		{
			SmartBody::util::log("No parameter named '%s' in BML '%s', ignoring...", attrName.c_str(), this->getName().c_str());
		}

		auto* stringAttribute = dynamic_cast<SmartBody::StringAttribute*>(attribute);
		if (stringAttribute)
		{
			const std::vector<std::string>& validValues = stringAttribute->getValidValues();
			if (!validValues.empty())
			{
				bool match = false;
				for (const auto & validValue : validValues)
				{
					if (validValue == attrValue)
					{
						match = true;
						break;
					}
				}
				if (!match)
				{
					SmartBody::util::log("Bad value '%s' for parameter '%s' in BML behavior '%s', setting to '%s' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), stringAttribute->getDefaultValue().c_str());
				}
				else
				{
					stringAttribute->setValue(attrValue);
				}
			}
		}
		
		auto* doubleAttribute = dynamic_cast<SmartBody::DoubleAttribute*>(attribute);
		if (doubleAttribute)
		{
			try 
			{
				auto val = boost::lexical_cast<double>(attrValue);
				if (doubleAttribute->getMin() > val)
				{
					SmartBody::util::log("Value '%s' for parameter '%s' in BML behavior '%s' is too small. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), doubleAttribute->getMin());			
					doubleAttribute->setValue(doubleAttribute->getMin());
				}
				else if (doubleAttribute->getMax() < val)
				{
					SmartBody::util::log("Value '%s' for parameter '%s' in BML behavior '%s' is too large. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), doubleAttribute->getMax());			
					doubleAttribute->setValue(doubleAttribute->getMax());
				}
			}
			catch(...)
			{ 
				SmartBody::util::log("Value '%s' for parameter '%s' in BML behavior '%s' is not a number. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), doubleAttribute->getDefaultValue());
				doubleAttribute->setValue(doubleAttribute->getDefaultValue());
			}
		}

		auto* intAttribute = dynamic_cast<SmartBody::IntAttribute*>(attribute);
		if (intAttribute)
		{
			try 
			{
				int val = boost::lexical_cast<int>(attrValue);
				if (intAttribute->getMin() > val)
				{
					SmartBody::util::log("Value '%s' for parameter '%s' in BML behavior '%s' is too small. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), intAttribute->getMin());			
					intAttribute->setValue(intAttribute->getMin());
				}
				else if (intAttribute->getMax() < val)
				{
					SmartBody::util::log("Value '%s' for parameter '%s' in BML behavior '%s' is too large. Setting to '%f' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), intAttribute->getMax());			
					intAttribute->setValue(intAttribute->getMax());
				}
			}
			catch(...)
			{ 
				SmartBody::util::log("Value '%s' for parameter '%s' in BML behavior '%s' is not an integer. Setting to '%d' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), intAttribute->getDefaultValue());
				doubleAttribute->setValue(intAttribute->getDefaultValue());
			}
		}

		auto* boolAttribute = dynamic_cast<SmartBody::BoolAttribute*>(attribute);
		if (boolAttribute)
		{
			if (attrValue == "true" || 
				attrValue == "TRUE")
			{
				boolAttribute->setValue(true);
			}
			else if (attrValue == "false" || 
				     attrValue == "FALSE")
			{
				boolAttribute->setValue(false);
			}
			else
			{
				SmartBody::util::log("Value '%s' for parameter '%s' in BML behavior '%s' is not a true/false value. Setting to '%s' instead.", attrValue.c_str(), attrName.c_str(), behaviorName.c_str(), boolAttribute->getDefaultValue() ? "true" : "false");
			}
		}
	}
}

std::vector<SmartBody::SBSyncPoint*> BMLObject::getSyncPointObjects()
{
	std::vector<SmartBody::SBSyncPoint*> syncPoints;

	std::string id = this->getStringAttribute("id");
	if (!id.empty())
	{
			auto* spStart = new SmartBody::SBSyncPoint(id + ":start");
			auto* spEnd = new SmartBody::SBSyncPoint(id + ":end");
			syncPoints.emplace_back(spStart);
			syncPoints.emplace_back(spEnd);
	}

	return syncPoints;
}

std::vector<SmartBody::SBTrigger*> BMLObject::getTriggers()
{
	std::vector<SmartBody::SBTrigger*> triggers;

	std::string start = this->getStringAttribute("start");
	if (!start.empty())
	{
		auto* trigger = new SmartBody::SBTrigger();
		trigger->setStartTag(start);
		triggers.emplace_back(trigger);
	}

	std::string end = this->getStringAttribute("end");
	if (!end.empty())
	{
		auto* trigger = new SmartBody::SBTrigger();
		trigger->setEndTag(end);
		triggers.emplace_back(trigger);
	}

	return triggers;

}

void BMLObject::start()
{
}

void BMLObject::stop()
{
}
