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
		UpdateBoundingBox();
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
		for (auto o : _obstacles) {
			if (o->Contains(steve)) return;
		}

		_obstacles.front()->AssignStevelet(steve);
		steve->FreezePhysics();
		steve->SetWorldPosition(GetWorldPosition());
		steve->MoveForward();
	}

	RN::Vector3 Level::CalculateEndPosition() {
		auto position = GetWorldPosition();
		for (auto o : _obstacles) {
			position.z += o->GetBoundingBox().maxExtend.z - o->GetBoundingBox().minExtend.z;
		}
		return position;
	}

	void Level::Update(float delta) {
		auto grabbables = World::GetSharedInstance()->GetGrabbableObjects();

		for (int i = 0; i < grabbables->GetCount(); i++) {
			auto entity = (*grabbables)[i]->Downcast<Stevelet>();
			if (entity) {
				if (_startTrigger.Contains(entity->GetWorldPosition())) {
					AssignStevelet(entity);
				}
			}
		}
	}

	void Level::UpdateBoundingBox() {

		auto start = GetWorldPosition();
		auto end = CalculateEndPosition();

		_startTrigger.position = (start + end) / 2;
		_startTrigger.minExtend = start - _startTrigger.position;
		_startTrigger.maxExtend = end - _startTrigger.position;

		if (_startTrigger.maxExtend.x < 0) std::swap(_startTrigger.minExtend.x, _startTrigger.maxExtend.x);
		if (_startTrigger.maxExtend.y < 0) std::swap(_startTrigger.minExtend.y, _startTrigger.maxExtend.y);
		if (_startTrigger.maxExtend.z < 0) std::swap(_startTrigger.minExtend.z, _startTrigger.maxExtend.z);

		auto scaling = _startTrigger.maxExtend - _startTrigger.minExtend;

		constexpr float MinDim = 0.8f;
		if (scaling.x > 0) {
			scaling.x = std::max(1.0f, 0.8f / scaling.x);
			_startTrigger.minExtend.x *= scaling.x;
		} else {
			_startTrigger.minExtend.x = -MinDim / 2.0f;
			_startTrigger.maxExtend.x = MinDim / 2.0f;
		}
		if (scaling.y > 0) {
			scaling.y = std::max(1.0f, 0.8f / scaling.y);
			_startTrigger.minExtend.y *= scaling.y;
		} else {
			_startTrigger.minExtend.y = -MinDim / 2.0f;
			_startTrigger.maxExtend.y = MinDim / 2.0f;
		}
		if (scaling.z > 0) {
			scaling.z = std::max(1.0f, 0.8f / scaling.z);
			_startTrigger.minExtend.z *= scaling.z;
		} else {
			_startTrigger.minExtend.z = -MinDim / 2.0f;
			_startTrigger.maxExtend.z = MinDim / 2.0f;
		}
	}
}
