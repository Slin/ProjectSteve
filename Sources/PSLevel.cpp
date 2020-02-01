//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSLevel.h"
#include "PSWorld.h"
#include <algorithm>

namespace PS
{
	RNDefineMeta(Level, RN::Entity)

	void Level::AddObstacle(Obstacle* obst) {
		auto position = CalculateEndPosition();
		position.z -= obst->GetBoundingBox().minExtend.z;
		obst->SetWorldPosition(position);
		_obstacles.push_back(obst);
	}

	void Level::FreeStevelet(Stevelet* steve, Obstacle* obs) {
		auto current = std::find(_obstacles.begin(), _obstacles.end(), obs);
		++current;
		if (current == _obstacles.end()) {
			steve->StopMovement();
			steve->ResumePhysics();
			return;
		}
		(*current)->AssignStevelet(steve);
	}

	void Level::AssignStevelet(Stevelet* steve) {
		_obstacles.front()->AssignStevelet(steve);
		steve->FreezePhysics();
		steve->MoveForward();
	}

	RN::Vector3 Level::CalculateEndPosition() {
		auto position = GetWorldPosition();
		for (auto o : _obstacles) {
			position.z += o->GetBoundingBox().maxExtend.z - o->GetBoundingBox().minExtend.z;
		}
		return position;
	}
}
