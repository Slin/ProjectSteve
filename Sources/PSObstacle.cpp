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

	Obstacle::Obstacle(float zLength, Level* parent) : Animatable(nullptr), _parent{ parent } {
		_isEmpty = true;
		auto aabb = GetBoundingBox();
		aabb.minExtend.z = -zLength / 2;
		aabb.maxExtend.z = zLength / 2;
		SetBoundingBox(aabb);
	}
	
	void Obstacle::AssignStevelet(Stevelet* steve) {
		_steves.push_back(steve);
	}

	bool Obstacle::Contains(Stevelet* steve) {
		return std::find(_steves.begin(), _steves.end(), steve) != _steves.end();
	}

	bool Obstacle::IsReached(float z) {
		return (z > GetZTreshold() - 0.05f) && _isEmpty;
	}

	float Obstacle::GetZTreshold() {
		return GetBoundingBox().position.z + GetBoundingBox().minExtend.z;
	}

	void Obstacle::Update(float delta) {
		PS::Animatable::Update(delta);

		for (Stevelet* &steve : _steves) {
			if (!steve) continue;
			if (steve->IsGrabbed()) {
				_parent->RemoveStevelet(steve);
				steve = nullptr;
				continue;
			}

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
