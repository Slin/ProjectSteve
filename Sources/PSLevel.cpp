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

	Level::Level() {
	/*	auto obs = new PS::Obstacle(0.5f, this);
		AddObstacle(obs);
		PS::World::GetSharedInstance()->AddLevelNode(obs->Autorelease(), false);*/
	}

	void Level::AddObstacle(Obstacle* obst) {
		auto position = CalculateEndPosition();
		obst->SetWorldPosition(position);
		_obstacles.push_back(obst);
		
		_startTrigger.position = GetWorldPosition() - RN::Vector3(0.0f, 0.0f, 0.0f);
		_startTrigger.minExtend = RN::Vector3(-0.25f, 0.0f, -0.25f);
		_startTrigger.maxExtend = RN::Vector3(0.25f, 1.5f, position.z - GetWorldPosition().z + 0.25f);
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
			if ((*it)->IsReached(steve->GetWorldPosition().z)) {
				auto position = RN::Vector3(GetWorldPosition().x, GetWorldPosition().y + 0.65f, (*it)->GetZTreshold() + 0.05f);
				steve->SetWorldPosition(position);
				(*it)->AssignStevelet(steve);
				break;
			}
		}

		steve->EnterObstacleCourse();
	}

	RN::Vector3 Level::CalculateEndPosition() {
		auto position = GetWorldPosition();
		for (auto o : _obstacles) {
			position.z += 0.5f;
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
