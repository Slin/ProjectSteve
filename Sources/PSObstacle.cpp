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
	RNDefineMeta(Obstacle, RN::Entity)

	Obstacle::Obstacle(RN::String const* modelName, const RN::String *collisionModelName, Level* parent) : RN::Entity(RN::Model::WithName(modelName)), _parent{ parent }
	{
		RN::Model *collisionModel = GetModel();
		if(collisionModelName != nullptr) collisionModel = RN::Model::WithName(collisionModelName);
		
		RN::PhysXMaterial *levelPhysicsMaterial = new RN::PhysXMaterial();
		RN::PhysXCompoundShape *levelShape = RN::PhysXCompoundShape::WithModel(collisionModel, levelPhysicsMaterial->Autorelease(), true);
		RN::PhysXStaticBody *levelBody = RN::PhysXStaticBody::WithShape(levelShape);
		levelBody->SetCollisionFilter(World::CollisionType::Level, World::CollisionType::All);
		AddAttachment(levelBody);
	}

	Obstacle::~Obstacle() {
		if (_effect) delete _effect;
	}

	void Obstacle::SetEffect(Effect* effect) {
		if (_effect) delete _effect;
		_effect = effect;
	}
	
	void Obstacle::SetZOriented(bool zOriented) {
		_isZOriented = zOriented;
		if (!_isZOriented) Rotate(RN::Vector3(90.0f, 0.0f, 0.0f));
	}

	void Obstacle::AssignStevelet(Stevelet* steve) {
		_steves.push_back(steve);
		if(_isZOriented) steve->EnterObstacleCourse({ GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z + 1.0f});
		else  steve->EnterObstacleCourse({ GetWorldPosition().x + 1.0f, GetWorldPosition().y, GetWorldPosition().z });
		_effect->executeChallenge(steve, this);
	}

	void Obstacle::FreeStevelet(Stevelet* steve) {
		_parent->FreeStevelet(steve, this);
		auto pos = std::find(_steves.begin(), _steves.end(), steve);
		*pos = nullptr;
	}

	bool Obstacle::Contains(Stevelet* steve) {
		return std::find(_steves.begin(), _steves.end(), steve) != _steves.end();
	}

	bool Obstacle::IsReached(float z) {
		return (z > GetZTreshold() - 0.1f);
	}

	float Obstacle::GetZTreshold() {
		if(_isZOriented) return GetBoundingBox().position.z + GetBoundingBox().minExtend.z;
		else return GetBoundingBox().position.x + GetBoundingBox().minExtend.x;
	}

	void Obstacle::Update(float delta) {
		RN::Entity::Update(delta);

		for (Stevelet* &steve : _steves) {
			if (!steve) continue;
			if (steve->IsGrabbed()) {
				_parent->RemoveStevelet(steve);
				steve = nullptr;
				continue;
			}

			auto aabb = GetBoundingBox();
			aabb.maxExtend.y = 2.0f;
			aabb.minExtend.y = 0.45f;
			if (_isZOriented) {
				aabb.maxExtend.z -= 0.05f;
				aabb.minExtend.z -= 0.05f;
			} else {
				aabb.maxExtend.x -= 0.05f;
				aabb.minExtend.x -= 0.05f;
			}
			if (!aabb.Contains(steve->GetWorldPosition())) {
				bool condition = _isZOriented ? (steve->GetWorldPosition().z > aabb.position.z + aabb.maxExtend.z) : (steve->GetWorldPosition().x > aabb.position.x + aabb.maxExtend.x);
				if (condition) {
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
