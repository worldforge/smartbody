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

#ifndef _SBCHARACTER_H_
#define _SBCHARACTER_H_

#include <sb/SBTypes.h>
#include <sb/sbm_character.hpp>
#include <sb/SBMotion.h>
#include <sb/SBCharacterFrameData.h>

namespace SmartBody {

class SBSkeleton;
class SBBehavior;
class SBSteerAgent;
class SBController;
class SBDiphone;
class SBReach;
class SBMotionGraph;
class SBParserListener;

class SBCharacter : public SbmCharacter
{
	public:
		SBAPI SBCharacter();
		SBAPI explicit SBCharacter(const std::string& name, const std::string& type = "");
		SBAPI ~SBCharacter() override;

		SBAPI const std::string& getName() const override;
		SBAPI void setName(const std::string& name) override;

		SBAPI void setType(const std::string& type);
		SBAPI std::string getType();
		
		SBAPI int setup() override;

		SBAPI int getNumControllers();

		SBAPI void setAutomaticPruning(bool val);
		SBAPI bool isAutomaticPruning();
		SBAPI void pruneControllers();

		SBAPI void setUseVisemeCurves(bool val);
		SBAPI bool isUseVisemeCurves();

		SBAPI float getVisemeTimeOffset();
		SBAPI void setVisemeTimeOffset(float val);

		SBAPI void setVoice(const std::string& type);
		SBAPI void setVoiceCode(const std::string& param);
		SBAPI const std::string getVoice();
		SBAPI const std::string& getVoiceCode();

		SBAPI void setVoiceBackup(const std::string& type);
		SBAPI void setVoiceBackupCode(const std::string& param);
		SBAPI const std::string& getVoiceBackup();
		SBAPI const std::string& getVoiceBackupCode();

		SBAPI SBController* getControllerByIndex(int i);
		SBAPI SBController* getControllerByName(const std::string& name);
		SBAPI std::vector<std::string> getControllerNames();
		SBAPI void addController(int index, SBController* controller);
		SBAPI void removeController(SBController* controller);

		SBAPI void startMotionRecord(double frameRate);
		SBAPI void stopMotionRecord(const std::string& motionName, const std::string& type);
		SBAPI void writeMotionRecord(const std::string& motionName, const std::string& type);

		SBAPI int getNumBehaviors();
		SBAPI SBBehavior* getBehavior(int num);
		//SBAPI std::vector<SBBehavior*>& getBehaviors();
		
		//SBAPI double getLastScheduledSpeechBehavior();
		//SBAPI std::string hasSpeechBehavior();

		SBAPI SBFaceDefinition* getFaceDefinition() override;
		SBAPI void setFaceDefinition(SBFaceDefinition* face) override;
		SBAPI virtual void updateDefaultFacePose();

		SBAPI void interruptFace(double seconds);

		SBAPI void notify(SBSubject* subject) override;
		SBAPI std::string getPostureName(); // get the current posture of character
		SBAPI void setDeformableMeshScale(double meshScale);
	    SBAPI void copy(SBCharacter* origChar);
		SBAPI void setReach(SmartBody::SBReach* reach);
		SBAPI SmartBody::SBReach* getReach();

		SBAPI void setMotionGraph(const std::string& moGraphName);
		SBAPI void startMotionGraph(const std::string& moNodeName);
		SBAPI void startMotionGraphWithPath(const std::vector<SrVec>& pathList);
		SBAPI void startMotionGraphRandomWalk();

		SBAPI void setUseJointConstraint(bool bUseConstraint);
		SBAPI bool getUseJointConstraint();
		SBAPI void addJointTrajectoryConstraint(const std::string& jointName, const std::string& refJointName);
		SBAPI TrajectoryRecord* getJointTrajectoryConstraint(const std::string& jointName);
		SBAPI std::vector<std::string> getJointConstraintNames();
		float getJointTrajBlendWeight() const { return jointTrajBlendWeight; }
		void setJointTrajBlendWeight(float val) { jointTrajBlendWeight = val; }

		SBAPI void createBlendShapeChannel(const std::string& channelName);

		SBAPI void setDeformableMeshName(const std::string& meshName);

		SBAPI const SBM_CharacterFrameDataMarshalFriendly & GetFrameDataMarshalFriendly();
		SBAPI std::string getReachAttachedPawnName(const std::string& reachType); // get the name of pawn that is currently picked up by the character. If there is no pawn attached, then the method return name="null".

		SBAPI bool getUseBlendFaceTextures();
		SBAPI void setUseBlendFaceTextures(bool);

		SBAPI bool getUseFaceTextures();
		SBAPI void setUseFaceTextures(bool);

		SBAPI void addParserListener(SBParserListener* parserListener);
		SBAPI void removeParserListener();
		SBAPI SBParserListener* getParserListener();

		bool isInLocomotion() const;

		void setLocomotionStatusProvider(std::function<bool()> provider);

	protected:
		void InitFrameDataMarshalFriendly();
		void FreeFrameDataJointsMarshalFriendly();
		void FreeFrameDataMarshalFriendly();

	protected:
		std::vector<SBBehavior*> _curBehaviors;	
		SmartBody::SBReach* _reach;
		SmartBody::SBMotionGraph* _curMotionGraph;
		std::map<std::string, TrajectoryRecord*> jointTrajMap;
		float jointTrajBlendWeight;
		bool useJointConstraint;
		SBM_CharacterFrameDataMarshalFriendly * frameDataMarshalFriendly;

		bool _useBlendFaceTextures;
		SBParserListener* _parserListener;
		std::function<bool()> _locomotionStatusProvider;

};

};

#endif
