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

#include "SBRealtimeManager.h"
#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBJoint.h>
#include <sb/SBSimulationManager.h>
#include <sb/SBAttribute.h>
#include "SBUtilities.h"

#include <iostream>
#include <sstream>


namespace SmartBody {

SBRealtimeManager::SBRealtimeManager() : SBService()
{
	setEnable(false);
	setName("Realtime");
	createBoolAttribute("debug", false, true, "Basic", 60, false, false, false, "Show debugging information.");
	createActionAttribute("dumpData", true, "Basic", 60, false, false, false, "Dump the realtime data.");
#if USE_PERCEPTIONNEURON > 0
	createBoolAttribute("usePerceptionNeuron", false, true, "Perception Neuron", 10, false, false, false, "Uses the Perception Neuron interface.");
	createStringAttribute("perceptionNeuronIP", "127.0.0.1", true, "Perception Neuron", 20, false, false, false, "Perception Neuron server IP.");
	createIntAttribute("perceptionNeuronPort", 7001, true, "Perception Neuron", 30, false, false, false, "Perception Neuron port.");
	createStringAttribute("perceptionNeuronStatus", "Disconnected", true, "Perception Neuron", 40, false, false, false, "Perception Neuron status.");
	
	
	m_sockTCPRef = 0;
	m_sockUDPRef = 0;
	_valuesBufferLength = 354;
	_valuesBuffer = new float[_valuesBufferLength];
	_frameCount = 0;

	_dataIndexMap[0] = "Hips/pos"; // position
	_dataIndexMap[1] = "Hips"; // rotation
	_dataIndexMap[2] = "RightUpLeg";
	_dataIndexMap[3] = "RightLeg";
	_dataIndexMap[4] = "RightFoot";
	_dataIndexMap[5] = "LeftUpLeg";
	_dataIndexMap[6] = "LeftLeg";
	_dataIndexMap[7] = "LeftFoot";
	_dataIndexMap[8] = "Spine";
	_dataIndexMap[9] = "Spine1";
	_dataIndexMap[10] = "Spine2";
	_dataIndexMap[11] = "Spine3";
	_dataIndexMap[12] = "Neck";
	_dataIndexMap[13] = "Head";
	_dataIndexMap[14] = "RightShoulder";
	_dataIndexMap[15] = "RightArm";
	_dataIndexMap[16] = "RightForeArm";
	_dataIndexMap[17] = "RightHand";
	_dataIndexMap[18] = "RightHandThumb1";
	_dataIndexMap[19] = "RightHandThumb2";
	_dataIndexMap[20] = "RightHandThumb3";
	_dataIndexMap[21] = "RightInHandIndex";
	_dataIndexMap[22] = "RightHandIndex1";
	_dataIndexMap[23] = "RightHandIndex2";
	_dataIndexMap[24] = "RightHandIndex3";
	_dataIndexMap[25] = "RightInHandMiddle";
	_dataIndexMap[26] = "RightHandMiddle1";
	_dataIndexMap[27] = "RightHandMiddle2";
	_dataIndexMap[28] = "RightHandMiddle3";
	_dataIndexMap[29] = "RightInHandRing";
	_dataIndexMap[30] = "RightHandRing1";
	_dataIndexMap[31] = "RightHandRing2";
	_dataIndexMap[32] = "RightHandRing3";
	_dataIndexMap[33] = "RightInHandPinky";
	_dataIndexMap[34] = "RightHandPinky1";
	_dataIndexMap[35] = "RightHandPinky2";
	_dataIndexMap[36] = "RightHandPinky3";
	_dataIndexMap[37] = "LeftShoulder";
	_dataIndexMap[38] = "LeftArm";
	_dataIndexMap[39] = "LeftForeArm";
	_dataIndexMap[40] = "LeftHand";
	_dataIndexMap[41] = "LeftHandThumb1";
	_dataIndexMap[42] = "LeftHandThumb2";
	_dataIndexMap[43] = "LeftHandThumb3";
	_dataIndexMap[44] = "LeftInHandIndex";
	_dataIndexMap[45] = "LeftHandIndex1";
	_dataIndexMap[46] = "LeftHandIndex2";
	_dataIndexMap[47] = "LeftHandIndex3";
	_dataIndexMap[48] = "LeftInHandMiddle";
	_dataIndexMap[49] = "LeftHandMiddle1";
	_dataIndexMap[50] = "LeftHandMiddle2";
	_dataIndexMap[51] = "LeftHandMiddle3";
	_dataIndexMap[52] = "LeftInHandRing";
	_dataIndexMap[53] = "LeftHandRing1";
	_dataIndexMap[54] = "LeftHandRing2";
	_dataIndexMap[55] = "LeftHandRing3";
	_dataIndexMap[56] = "LeftInHandPinky";
	_dataIndexMap[57] = "LeftHandPinky1";
	_dataIndexMap[58] = "LeftHandPinky2";
	_dataIndexMap[59] = "LeftHandPinky3";

	std::stringstream strstr;
	for (std::map<int, std::string>::iterator iter = _dataIndexMap.begin();
		 iter != _dataIndexMap.end();
		 iter++)
	{
		const std::string value = (*iter).second;
		strstr << value << " ";
	}
	this->setChannelNames(strstr.str());

#endif
	useFrame = false;
}

SBRealtimeManager::~SBRealtimeManager()
{
#if USE_PERCEPTIONNEURON > 0
	stopPerceptionNeuron();
	delete [] _valuesBuffer;
#endif
}

void SBRealtimeManager::setEnable( bool val )
{	
	SBService::setEnable(val);	
	if (val)
		start();
	else
		stop();
}

bool SBRealtimeManager::isEnable()
{
	return SBService::isEnable();
}

void SBRealtimeManager::start()
{
	channelTable.clear();

	initConnection();
}

void SBRealtimeManager::stop()
{
	stopConnection();
}


void SBRealtimeManager::update( double time )
{
	if (!this->isEnable())
		false;

#if USE_PERCEPTIONNEURON > 0
	if (this->getStringAttribute("perceptionNeuronStatus") == "Connected")
	{
		if (m_sockTCPRef != 0)
		{
			//BRCommandFetchDataFromServer(m_sockTCPRef, Cmd_AvatarCount);
			//BRCommandFetchAvatarDataFromServer(m_sockTCPRef, 0, Cmd_AvatarCount);
		}
	}
#endif
}

void SBRealtimeManager::initConnection()
{
}

void SBRealtimeManager::stopConnection()
{
}

void SBRealtimeManager::setChannelNames(const std::string& channels)
{
	channelTable.clear();
	channelNames.clear();

	useFrame = false;

	std::vector<std::string> tokens;
	SmartBody::util::tokenize(channels, tokens, " ");
	
	for (size_t c = 0; c < tokens.size(); c++)
	{
		channelNames.push_back(tokens[c]);
		channelTable.insert(std::pair<std::string, std::string>(tokens[c], ""));
	}
}

void SBRealtimeManager::setChannelMetadata(const std::string& channelsandamountsperchannel)
{
	channelMetadataMap.clear();
	channelMetadataOrder.clear();

	useFrame = true;

	std::vector<std::string> tokens;
	SmartBody::util::tokenize(channelsandamountsperchannel, tokens, " ");

	int curIndex = 0;
	for (size_t c = 0; c < tokens.size(); c++)
	{
		std::string& channelName = tokens[c];
		if (c + 1 < tokens.size())
		{
			int numParams = atoi(tokens[c + 1].c_str());
			if (numParams < 1)
			{
				SmartBody::util::log("Params needs to contain a value > 0: %s", channelsandamountsperchannel.c_str());
			}

			channelIndexStruct s;
			s.index = curIndex;
			s.size = numParams;

			channelMetadataMap.insert(std::pair<std::string, channelIndexStruct>(channelName, s));
			channelMetadataOrder.push_back(channelName);
			curIndex += numParams;
		}
		else
		{
			SmartBody::util::log("Metadata needs to contain (channelname numparams): %s", channelsandamountsperchannel.c_str());
		}
		c++;
	}
}

void SBRealtimeManager::setDataFrame(const std::string& frameDataStr)
{
	std::vector<std::string> tokens;
	SmartBody::util::tokenize(frameDataStr, tokens, " ");
	if (frameData.size() < tokens.size())
		frameData.resize(tokens.size());

	for (size_t t = 0; t < tokens.size(); t++)
	{
		double val = atof(tokens[t].c_str());
		frameData[t] = val;
	}
}

std::vector<std::string>& SBRealtimeManager::getChannelNames()
{
	return channelNames;
}

void SBRealtimeManager::setData(const std::string& channel, const std::string& data)
{
	if (channelTable.find(channel) != channelTable.end())
	{
		channelTable[channel] = data;
	}
	else
	{
		if (this->getBoolAttribute("debug"))
			SmartBody::util::log("Cannot find channel named: %s", channel.c_str());
	}
}

std::string SBRealtimeManager::getData(const std::string& channel)
{
	std::string data = "";
	if (channelTable.find(channel) != channelTable.end())
	{
		data = channelTable[channel];
	}
	else
	{
		if (this->getBoolAttribute("debug"))
			SmartBody::util::log("Cannot find channel named: %s", channel.c_str());
	}
	
	return data;
}

SrQuat SBRealtimeManager::getDataQuat(const std::string& channel)
{
	SrQuat quat;

	if (useFrame)
	{
		// get the index of the channel
		std::map<std::string, channelIndexStruct>::iterator iter = channelMetadataMap.find(channel);
		if (iter == channelMetadataMap.end())
			return quat;
		if ((*iter).second.size != 4)
			return quat;
		int index = (*iter).second.index;
		double w = frameData[index];
		double x = frameData[index + 1];
		double y = frameData[index + 2];
		double z = frameData[index + 2];
		quat.set((float)w, (float)x, (float)y, (float)z);
		return quat;
	}

	std::string data = getData(channel);
	if (data == "")
		return quat;

	std::vector<std::string> tokens;
	SmartBody::util::tokenize(data, tokens, " ");
	if (tokens.size() < 4)
		return quat;

	double w = atof(tokens[0].c_str());
	double x = atof(tokens[1].c_str());
	double y = atof(tokens[2].c_str());
	double z = atof(tokens[3].c_str());

	quat.set((float) w, (float) x, (float) y, (float) z);

	return quat;
}

SrMat SBRealtimeManager::getDataMat(const std::string& channel)
{
	SrMat mat;

	if (useFrame)
	{
		// get the index of the channel
		std::map<std::string, channelIndexStruct>::iterator iter = channelMetadataMap.find(channel);
		if (iter == channelMetadataMap.end())
			return mat;
		if ((*iter).second.size != 16)
			return mat;
		int index = (*iter).second.index;
		for (int m = 0; m < 16; m++)
			mat.set(m, (float)frameData[index + m]);
		return mat;
	}
	
	std::string data = getData(channel);
	if (data == "")
		return mat;

	std::vector<std::string> tokens;
	SmartBody::util::tokenize(data, tokens, " ");
	if (tokens.size() < 16)
		return mat;

	for (int m = 0; m < 16; m++)
		mat.set(m, (float) atof(tokens[m].c_str()));
	
	return mat;
}

SrVec SBRealtimeManager::getDataVec(const std::string& channel)
{
	SrVec vec;

	if (useFrame)
	{
		// get the index of the channel
		std::map<std::string, channelIndexStruct>::iterator iter = channelMetadataMap.find(channel);
		if (iter == channelMetadataMap.end())
			return vec;
		if ((*iter).second.size != 3)
			return vec;
		int index = (*iter).second.index;
		vec[0] = (float) frameData[index];
		vec[1] = (float) frameData[index + 1];
		vec[2] = (float) frameData[index + 2];
		return vec;
	}


	std::string data = getData(channel);
	if (data == "")
		return vec;

	std::vector<std::string> tokens;
	SmartBody::util::tokenize(data, tokens, " ");
	if (tokens.size() < 3)
		return vec;

	double x = atof(tokens[0].c_str());
	double y = atof(tokens[1].c_str());
	double z = atof(tokens[2].c_str());

	vec.set((float) x, (float) y, (float) z);

	return vec;
}

double SBRealtimeManager::getDataDouble(const std::string& channel)
{
	if (useFrame)
	{
		// get the index of the channel
		std::map<std::string, channelIndexStruct>::iterator iter = channelMetadataMap.find(channel);
		if (iter == channelMetadataMap.end())
			return 0.0;
		int index = (*iter).second.index;
		return frameData[index];
	}

	double val = 0.0;
	std::string data = getData(channel);
	if (data == "")
		return val;

	val = atof(data.c_str());

	return val;
}

int SBRealtimeManager::getDataInt(const std::string& channel)
{
	if (useFrame)
	{
		// get the index of the channel
		std::map<std::string, channelIndexStruct>::iterator iter = channelMetadataMap.find(channel);
		if (iter != channelMetadataMap.end())
			return 0;
		int index = (*iter).second.index;
		return (int) frameData[index];
	}

	int val = 0;
	std::string data = getData(channel);
	if (data == "")
		return val;

	val = atoi(data.c_str());

	return val;
}

void SBRealtimeManager::notify(SBSubject* subject)
{
	SBService::notify(subject);

	SBAttribute* attribute = dynamic_cast<SBAttribute*>(subject);
	if (attribute)
	{
		const std::string name = attribute->getName();
		if (name == "dumpData")
		{
			for (std::map<std::string, std::string>::iterator iter =  channelTable.begin();
				 iter != channelTable.end();
				 iter++)
			{
				SmartBody::util::log("[%s] = %s", (*iter).first.c_str(), (*iter).second.c_str());
			}
		}
#if USE_PERCEPTIONNEURON > 0
		else if (name == "usePerceptionNeuron")
		{
			bool running = isPerceptionNeuronRunning();
			bool isEnabled = this->getBoolAttribute("usePerceptionNeuron");
			if (isEnabled)
			{
				if (running)
				{
					restartPerceptionNeuron();
				}
				else
				{
					startPerceptionNeuron();
				}
			}
			else
			{
				if (running)
				{
					stopPerceptionNeuron();
				}
				else
				{
					SmartBody::util::log("PerceptionNeuron already stopped.");
				}
			}
		}
#else
		else if (name == "usePerceptionNeuron")
		{
			SmartBody::util::log("Perception neuron module not enabled. Please set USE_PERCEPTIONNEURON and rebuild.");
		}
#endif
	}

}

#if USE_PERCEPTIONNEURON > 0

void SBRealtimeManager::startPerceptionNeuron()
{
	BRRegisterFrameDataCallback(this, myFrameDataReceived);
	BRRegisterCalculationDataCallback(this, myCalculationDataReceived);
	BRRegisterSocketStatusCallback(this, mySocketStatusChanged);

	if (BRGetSocketStatus(m_sockTCPRef) == CS_Running)
	{
		SmartBody::util::log("Perception neuron already connected...");
		return;
	}
	SmartBody::util::log("Attempting to connect to %s:%d", this->getStringAttribute("perceptionNeuronIP").c_str(), this->getIntAttribute("perceptionNeuronPort"));
	m_sockTCPRef = BRConnectTo((char*) this->getStringAttribute("perceptionNeuronIP").c_str(), this->getIntAttribute("perceptionNeuronPort"));
	if (m_sockTCPRef == 0)
	{
		char* errorMessage = BRGetLastErrorMessage();
		SmartBody::util::log("Error message from Perception Neuron: %s", errorMessage);
		this->setStringAttribute("perceptionNeuronStatus", "Disconnected");
		return;
	}
	m_sockUDPRef = BRStartUDPServiceAt(7001);
	//BRRegisterAutoSyncParmeter(m_sockTCPRef, Cmd_AvatarCount);
	SmartBody::util::log("Connected to Perception Neuron!");


	this->setStringAttribute("perceptionNeuronStatus", "Connected");

	_frameCount = 0;
	
		
}

void SBRealtimeManager::restartPerceptionNeuron()
{
	stopPerceptionNeuron();
	startPerceptionNeuron();
}

void SBRealtimeManager::stopPerceptionNeuron()
{
	SocketStatus status1 = BRGetSocketStatus(m_sockTCPRef);
	BRCloseSocket(m_sockTCPRef);

	SocketStatus status2 = BRGetSocketStatus(m_sockUDPRef);
	BRCloseSocket(m_sockUDPRef);
}

bool SBRealtimeManager::isPerceptionNeuronRunning()
{
	if (BRGetSocketStatus(m_sockTCPRef) == CS_Running)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SBRealtimeManager::myFrameDataReceived(void* customedObj, SOCKET_REF sender, BvhDataHeader* header, float* data)
{
	SBRealtimeManager* realtimeManager = (SBRealtimeManager*) customedObj;

	if (header->DataCount != realtimeManager->_valuesBufferLength)
	{
		delete [] realtimeManager->_valuesBuffer;
		realtimeManager->_valuesBuffer = new float[header->DataCount];
	}

	memcpy(&realtimeManager->_valuesBuffer[0], data, header->DataCount * sizeof(float));
	if (realtimeManager->m_sockTCPRef == sender)
	{
		realtimeManager->_frameCount++;

	}

	if (realtimeManager->getBoolAttribute("debug"))
	{
		for (int d = 0; d < header->DataCount; d += 3)
		{
			SmartBody::util::log("%f %f %f", realtimeManager->_valuesBuffer[d], realtimeManager->_valuesBuffer[d + 1], realtimeManager->_valuesBuffer[d + 2]);
		}
	}
	
	bool useDisplacement = header->WithDisp;
	bool useReference = header->WithReference;
	int dataCount = header->DataCount;

	if (!useDisplacement)
	{
		// store the data in the realtime manager
		for (size_t c = 0; c < realtimeManager->_dataIndexMap.size(); c++)
		{
			if (realtimeManager->_valuesBufferLength + 1> (c * 3))
			{
				if (c == 0)
				{
					// write the position into (jointname)/pos:
					std::stringstream strstr;
					strstr << realtimeManager->_valuesBuffer[c * 3] << " " << realtimeManager->_valuesBuffer[c * 3 + 1] << " " << realtimeManager->_valuesBuffer[c * 3 + 2];
					realtimeManager->setData(realtimeManager->_dataIndexMap[c], strstr.str());
				}
				else
				{
					// write the rotation into (jointname)/rot:
					int index = c * 3;
					SrMat yrot;
					yrot.roty(realtimeManager->_valuesBuffer[index] * SR_PI / 180.0);
					SrMat xrot;
					xrot.rotx(realtimeManager->_valuesBuffer[index + 1] * SR_PI / 180.0);
					SrMat zrot;
					zrot.rotz(realtimeManager->_valuesBuffer[index + 2] * SR_PI / 180.0);
				
					SrMat finalMat = zrot * xrot * yrot;
					SrQuat finalQuat(finalMat);
			
					std::stringstream strstr2;
					strstr2 << finalQuat.w << " " << finalQuat.x << " " << finalQuat.y << " " << finalQuat.z;
					realtimeManager->setData(realtimeManager->_dataIndexMap[c], strstr2.str());
				}
			}
		}
	}
	else
	{
		// store the data in the realtime manager
		for (size_t c = 0; c < realtimeManager->_dataIndexMap.size(); c++)
		{
			if (realtimeManager->_valuesBufferLength + 1 > (c * 6))
			{
				if (c == 0)
				{
					// write the position into (jointname)/pos:
					std::stringstream strstr;
					strstr << realtimeManager->_valuesBuffer[0] << " " << realtimeManager->_valuesBuffer[1] << " " << realtimeManager->_valuesBuffer[2];
					realtimeManager->setData(realtimeManager->_dataIndexMap[c], strstr.str());

					int index = 3;
					// write the rotation into (jointname)/rot:
					SrMat yrot;
					yrot.roty(realtimeManager->_valuesBuffer[index] * SR_PI / 180.0);
					SrMat xrot;
					xrot.rotx(realtimeManager->_valuesBuffer[index + 1] * SR_PI / 180.0);
					SrMat zrot;
					zrot.rotz(realtimeManager->_valuesBuffer[index + 2] * SR_PI / 180.0);
				
					SrMat finalMat = zrot * xrot * yrot;
					SrQuat finalQuat(finalMat);
			
					std::stringstream strstr2;
					strstr2 << finalQuat.w << " " << finalQuat.x << " " << finalQuat.y << " " << finalQuat.z;
					realtimeManager->setData(realtimeManager->_dataIndexMap[1], strstr2.str());



				}
				else
				{
					// write the rotation into (jointname)/rot:
					int index = c * 6 + 3;
					SrMat yrot;
					yrot.roty(realtimeManager->_valuesBuffer[index] * SR_PI / 180.0);
					SrMat xrot;
					xrot.rotx(realtimeManager->_valuesBuffer[index + 1] * SR_PI / 180.0);
					SrMat zrot;
					zrot.rotz(realtimeManager->_valuesBuffer[index + 2] * SR_PI / 180.0);
				
					SrMat finalMat = zrot * xrot * yrot;
					SrQuat finalQuat(finalMat);
			
					std::stringstream strstr2;
					strstr2 << finalQuat.w << " " << finalQuat.x << " " << finalQuat.y << " " << finalQuat.z;
					realtimeManager->setData(realtimeManager->_dataIndexMap[c + 1], strstr2.str());
				}
			}
		}
	}
	
}

void SBRealtimeManager::myCalculationDataReceived(void* customedObj, SOCKET_REF sender, CalcDataHeader* pack, float* data)
{
	/*
	if (pack->CommandId == Cmd_BoneSize)
	{
		SmartBody::util::log("Cmd_BoneSize");
	}
	else if (pack->CommandId == Cmd_AvatarName)
	{
		SmartBody::util::log("Cmd_AvatarName");
	}
	else if (pack->CommandId == Cmd_FaceDirection)
	{
		SmartBody::util::log("Cmd_BoneSize");
	}
	else if (pack->CommandId == Cmd_DataFrequency)
	{
		SmartBody::util::log("Cmd_DataFrequency");
	}
	else if (pack->CommandId == Cmd_BvhInheritanceTxt)
	{
		SmartBody::util::log("Cmd_BvhInheritanceTxt");
	}
	else if (pack->CommandId == Cmd_AvatarCount)
	{
		SmartBody::util::log("Cmd_AvatarCount");
	}
	else if (pack->CommandId == Cmd_CombinationMode)
	{
		SmartBody::util::log("Cmd_CombinationMode");
	}
	else if (pack->CommandId == Cmd_RegisterEvent)
	{
		SmartBody::util::log("Cmd_RegisterEvent");
	}
	else if (pack->CommandId == Cmd_UnRegisterEvent)
	{
		SmartBody::util::log("Cmd_UnRegisterEvent");
	}
	*/
}

void SBRealtimeManager::mySocketStatusChanged(void* customedObj, SOCKET_REF sender, SocketStatus status, char* message)
{
	std::string currentStatus =  "unknown";
	if (status == CS_Running)
	{
		currentStatus = "Running";
	}
	else if (status == CS_Running)
	{
		currentStatus = "Starting";
	}
	else if (status == CS_OffWork)
	{
		currentStatus = "OffWork";
	}
      
	SmartBody::util::log("Socket status: %s", currentStatus.c_str());
	if (message)
	{
		SmartBody::util::log("Socket message: %s", message);
	}
}

#endif

}

