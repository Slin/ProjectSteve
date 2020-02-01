//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSLevel.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Level, RN::Entity)

	void Level::AddObstacle(Obstacle* obst) {
		auto position = GetWorldPosition();

		for (auto o : _obstacles) {
			position.z += o->GetBoundingBox().maxExtend.z - o->GetBoundingBox().minExtend.z;
		}

		position.z -= obst->GetBoundingBox().minExtend.z;
		obst->SetWorldPosition(position);
		_obstacles.push_back(obst);
	}

	void Level::FreeSteve(Stevelet* steve, Obstacle* obs) {
		//TODO: Assign Steve to next obstacle
	}
}
