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

	void Obstacle::Update(float delta) {
		PS::Animatable::Update(delta);

		for (Stevelet* &steve : _steves) {
			auto aabb = GetBoundingBox();
			if (!aabb.Contains(steve->GetWorldPosition())) {
				if (steve->GetWorldPosition().x > aabb.position.x + aabb.maxExtend.x) {
					_parent->FreeSteve(steve, this);
				} else {
					World::GetSharedInstance()->RemoveLevelNode(steve);
					steve = nullptr;
				}
			}
		}
	}
}
