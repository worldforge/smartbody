/*
 Copyright (C) 2020 Erik Ogenvik
 Copyright (C) 2017 University of Southern California

 SmartBody-lib is free software: you can redistribute it and/or
 modify it under the terms of the Lesser GNU General Public License
 as published by the Free Software Foundation, version 3 of the
 license.

 SmartBody-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 Lesser GNU General Public License for more details.

 You should have received a copy of the Lesser GNU General Public
 License along with SmarBody-lib.  If not, see:
     http://www.gnu.org/licenses/lgpl-3.0.txt
 */

#ifndef SMARTBODY_SBRENDERSCENELISTENER_H
#define SMARTBODY_SBRENDERSCENELISTENER_H

#include <utility>

#include "sb/SBSceneListener.h"
#include "sb/SBObserver.h"
#include "sbm/SBRenderScene.h"

namespace SmartBody {

class SBRenderSceneListener : public SmartBody::SBSceneListener {
public:
	explicit SBRenderSceneListener(SBRenderScene& renderScene);

	void OnCharacterCreate(const std::string& name, const std::string& objectClass) override;

	void OnCharacterDelete(const std::string& name) override;

	void OnCharacterUpdate(const std::string& name) override;

	void OnPawnCreate(const std::string& name) override;

	void OnPawnDelete(const std::string& name) override;

	void OnSimulationUpdate() override;

protected:

	struct Observer : public SmartBody::SBObserver {
		explicit Observer(SBRenderScene::Renderable& renderable, SBRenderSceneListener& parent)
				: renderable(renderable),
				  parent(parent) {}

		SBRenderSceneListener& parent;
		SBRenderScene::Renderable& renderable;

		void notify(SBSubject* subject) override;

	};

	SBRenderScene& mRenderScene;
	std::map<std::string, Observer> mObservers;
};

}


#endif //SMARTBODY_SBRENDERSCENELISTENER_H
