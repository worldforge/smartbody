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

#ifndef _SBREALTIMEMANAGER_H_
#define _SBREALTIMEMANAGER_H_

#include <sb/SBTypes.h>
#include <sb/SBService.h>
#include <sr/sr_quat.h>
#include <string>
#include <sb/SBSubject.h>

#if defined(LINUX_BUILD) || defined(MAC_BUILD)
#elif defined (WIN_BUILD)
#define USE_PERCEPTIONNEURON 0
#endif





#if USE_PERCEPTIONNEURON > 0
#include <Windows.h>
#include "NeuronDataReader.h"
//#include "SocketCommand.h"
#endif

namespace SmartBody {


class SBRealtimeManager : public SBService
{
	public:
		SBAPI SBRealtimeManager();
		SBAPI ~SBRealtimeManager();

		SBAPI void setEnable(bool val) override;
		SBAPI bool isEnable() override;
		SBAPI void start() override;
		SBAPI void stop() override;

		// string-based data
		SBAPI virtual void setChannelNames(const std::string& names);
		SBAPI std::vector<std::string>& getChannelNames();

		// floating point data
		SBAPI virtual void setChannelMetadata(const std::string& names);
		SBAPI virtual void setDataFrame(const std::string& frameData);
				
		SBAPI void notify(SBSubject* subject) override;

		SBAPI void initConnection();
		SBAPI void stopConnection();
		SBAPI void update(double time) override;
		SBAPI void setData(const std::string& channel, const std::string& data);
		SBAPI std::string getData(const std::string& channel);
		SBAPI double getDataDouble(const std::string& channel);
		SBAPI int getDataInt(const std::string& channel);
		SBAPI SrVec getDataVec(const std::string& channel);
		SBAPI SrQuat getDataQuat(const std::string& channel);
		SBAPI SrMat getDataMat(const std::string& channel);

#if USE_PERCEPTIONNEURON > 0
		void startPerceptionNeuron();
		void restartPerceptionNeuron();
		void stopPerceptionNeuron();
		bool isPerceptionNeuronRunning();

		static void CALLBACK myFrameDataReceived(void* customedObj, SOCKET_REF sender, BvhDataHeader* header, float* data);
		static void CALLBACK myCalculationDataReceived(void* customedObj, SOCKET_REF sender, CalcDataHeader* pack, float* data);
		static void CALLBACK mySocketStatusChanged(void* customedObj, SOCKET_REF sender, SocketStatus status, char* message);
#endif

	protected:
		// string-based data
		std::vector<std::string> blendShapeNames;
		std::map<std::string, std::string> channelTable;
		std::vector<std::string> channelNames;

		// floating point data
		struct channelIndexStruct
		{
			int index;
			int size;
		};
		std::map<std::string, channelIndexStruct> channelMetadataMap;
		std::vector<std::string> channelMetadataOrder;
		bool useFrame;

		std::vector<channelIndexStruct> channelDataIndex;
		std::vector<double> frameData;
#if USE_PERCEPTIONNEURON > 0
		SOCKET_REF m_sockTCPRef;
		SOCKET_REF m_sockUDPRef;
		float* _valuesBuffer;
		int _valuesBufferLength;
		int _frameCount;
		std::map<int, std::string> _dataIndexMap;
#endif
};

}

#endif
