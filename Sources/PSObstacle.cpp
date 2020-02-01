//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSObstacle.h"
#include "PSWorld.h"
#include "PSLevel.h"

namespace PS
{
	RNDefineMeta(Obstacle, Animatable)
	
	void Obstacle::AssignStevelet(Stevelet* steve) {
		_steves.push_back(steve);
	}

	bool Obstacle::Contains(Stevelet* steve) {
		return std::find(_steves.begin(), _steves.end(), steve) != _steves.end();
	}

	void Obstacle::Update(float delta) {
		PS::Animatable::Update(delta);

		for (Stevelet* &steve : _steves) {
			if (!steve) continue;
			auto aabb = GetBoundingBox();
			if (!aabb.Contains(steve->GetWorldPosition())) {
				if (steve->GetWorldPosition().z > aabb.position.z + aabb.maxExtend.z) {
					_parent->FreeStevelet(steve, this);
					steve = nullptr;
				} else {
					steve->Kill();
					steve = nullptr;
				}
			}
		}


		_steves.erase(
			std::remove_if(_steves.begin(), _steves.end(), [](Stevelet const* steve) { return steve == nullptr; }),
			_steves.end());
	}
}
