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

#ifndef _SBMOTIONBLENDBASE_H
#define _SBMOTIONBLENDBASE_H

#include <sb/SBTypes.h>
#include <sb/SBAnimationState.h>
class MeCtBlendEngine;

namespace SmartBody {

	class SBMotionBlendBase : public SBAnimationBlend
	{
		public:		
			//SBAPI SBMotionBlendBase();
			SBAPI SBMotionBlendBase(SBScene& scene, const std::string& name, const std::string& skelName, int dimension);
			SBAPI ~SBMotionBlendBase();			

			
			SBAPI const std::string& getSkeleton();
			SBAPI const std::string& getInterpType();
			SBAPI const std::string& getBlendType();

			SBAPI virtual void addMotion(const std::string& motion, std::vector<double>& parameter);
			SBAPI virtual void setMotionParameter(const std::string& motion, std::vector<double>& parameter);
			SBAPI virtual std::vector<double> getMotionParameter(const std::string& motion);

			SBAPI void removeMotion(const std::string& motionName) override;
			SBAPI void buildBlendBase(const std::string& motionParameter, const std::string& interpolatorType, bool copySimplex);


			SBAPI bool getWeightsFromParameters(double x, std::vector<double>& weights) override;
			SBAPI bool getWeightsFromParameters(double x, double y, std::vector<double>& weights) override;
			SBAPI bool getWeightsFromParameters(double x, double y, double z, std::vector<double>& weights) override;
			SBAPI void getParametersFromWeights(float& x, std::vector<double>& weights) override;
			SBAPI void getParametersFromWeights(float& x, float& y, std::vector<double>& weights) override;
			SBAPI void getParametersFromWeights(float& x, float& y, float& z, std::vector<double>& weights) override;
			SBAPI void addTetrahedron(const std::string& motion1, const std::string& motion2, const std::string& motion3, const std::string& motion4);

			SBAPI std::string saveToString() override;

		protected:
			std::unique_ptr<MeCtBlendEngine> blendEngine;
			std::string interpType;		
			std::string skeletonName;
			std::string blendType;

	};
}
#endif // namespace