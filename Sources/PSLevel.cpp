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
		obst->SetWorldPosition(position);
		_obstacles.push_back(obst);
		obst->SetZOriented(_isZOriented);

		_startTrigger.position = GetWorldPosition() - RN::Vector3(0.0f, 0.0f, 0.0f);
		_startTrigger.minExtend = RN::Vector3(-0.25f, 0.0f, -0.25f);
		if (_isZOriented) {
			_startTrigger.maxExtend = RN::Vector3(0.25f, 1.5f, position.z - GetWorldPosition().z + 0.25f);
		} else {
			_startTrigger.maxExtend = RN::Vector3(position.x - GetWorldPosition().x + 0.25f, 1.5f, 0.25f);
		}
	}

	void Level::FreeStevelet(Stevelet* steve, Obstacle* obs) {
		auto current = std::find(_obstacles.begin(), _obstacles.end(), obs);
		++current;
		if (current == _obstacles.end()) {
			_winners.push_back({ 2.0f, steve });
			RemoveStevelet(steve);
			return;
		}
		(*current)->AssignStevelet(steve);
	}

	void Level::RemoveStevelet(Stevelet* steve) {
		steve->LeaveObstacleCourse();
	}

	void Level::AssignStevelet(Stevelet* steve) {
		for (auto o : _obstacles) {
			if (o->Contains(steve)) return;
		}

		_winners.erase(
			std::remove_if(
				_winners.begin(), 
				_winners.end(), 
				[](std::tuple<float, Stevelet*> t) { return std::get<float>(t) < RN::k::EpsilonFloat; }),
			_winners.end());
		for (auto tuple : _winners) {
			if (steve == std::get<Stevelet*>(tuple)) return;
		}

		for (auto it = _obstacles.rbegin(); it != _obstacles.rend(); ++it) {
			auto axisTreshold = _isZOriented ? steve->GetWorldPosition().z : steve->GetWorldPosition().x;
			if ((*it)->IsReached(axisTreshold)) {
				auto position = RN::Vector3(GetWorldPosition().x, GetWorldPosition().y + 0.65f, (*it)->GetZTreshold() + 0.05f);
				if(!_isZOriented) position = RN::Vector3((*it)->GetZTreshold() + 0.05f, GetWorldPosition().y + 0.65f, GetWorldPosition().z);
				steve->SetWorldPosition(position);
				(*it)->AssignStevelet(steve);
				break;
			}
		}
	}

	RN::Vector3 Level::CalculateEndPosition() {
		auto position = GetWorldPosition();
		for (auto o : _obstacles) {
			if(_isZOriented) position.z += 0.5f;
			else position.x += 0.5f;
		}
		return position;
	}

	void Level::Update(float delta) {
		for (auto& t : _winners) {
			std::get<float>(t) -= delta;
		}

		auto grabbables = World::GetSharedInstance()->GetGrabbableObjects();

		for (int i = 0; i < grabbables->GetCount(); i++) {
			auto entity = (*grabbables)[i]->Downcast<Stevelet>();
			if (entity) {
				if (!entity->IsGrabbed() && _startTrigger.Contains(entity->GetWorldPosition())) {
					AssignStevelet(entity);
				}
			}
		}
	}
}
