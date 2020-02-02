//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSStevelet.h"
#include "PSWorld.h"
#include "PSSpawner.h"

namespace PS
{
	RNDefineMeta(Stevelet, Animatable)
	
	Stevelet::Stevelet() : Animatable(RNCSTR("sprites/stevelet/0000000.png")), _isMoving(false)
	{
		RN::PhysXMaterial *material = new RN::PhysXMaterial();
		RN::PhysXShape *shape = RN::PhysXSphereShape::WithRadius(0.15, material);
		_physicsBody = RN::PhysXDynamicBody::WithShape(shape, 0.5f);
		_physicsBody->SetCollisionFilter(World::CollisionType::Players, World::CollisionType::Level);
		AddAttachment(_physicsBody);
	}

	void Stevelet::Kill() {
		_isMoving = false;

		if (_isGrabbed) {
			LeaveObstacleCourse();
			return;
		}

		_spawner->ReturnToPool(this);
	}

	void Stevelet::Vanish() {
		SetWorldPosition({ 0, 0, -42 });
	}

	void Stevelet::Jump(float intensity) {
		_physicsBody->ApplyForce({ 0, intensity, 0 });
	}

	void Stevelet::ResetVelocity() {
		_targetVelocity = _defaultVelocity;
	}

	void Stevelet::SetVelocity(float velocity) {
		_targetVelocity = velocity;
	}

	void Stevelet::EnterObstacleCourse()
	{
		_physicsBody->SetLinearVelocity(RN::Vector3());
		SetTargetPosition(RN::Vector3(GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z + 100.0f));
	}

	void Stevelet::LeaveObstacleCourse()
	{
		float offset = RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(1.3f, 2.4f);
		SetTargetPosition(RN::Vector3(GetWorldPosition().x - offset, GetWorldPosition().y, GetWorldPosition().z));
	}

	void Stevelet::Update(float delta)
	{
		delta = std::min(0.05f, delta);
		Animatable::Update(delta);
		
		RN::Vector3 direction = _targetPosition - GetWorldPosition();
		direction.y = 0.0f;
		if(direction.GetLength() > 0.2f)
		{
			if(_isMoving)
			{
				if(_physicsBody->GetLinearVelocity().GetLength() < _targetVelocity)
					_physicsBody->ApplyForce(direction.Normalize() * 0.08f);
			}
		}
		else
		{
			_isMoving = false;
		}
		
		if(!_isMoving)
		{
			SetTargetPosition(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomVector3Range(RN::Vector3(-1.7f, 0.15f, -1.7f), RN::Vector3(1.7f, 0.15f, 1.7f)));
		}
		
		if(_isGrabbed)
		{
			_physicsBody->SetLinearVelocity(RN::Vector3(0.0f, 9.81f*delta, 0.0f));
		}
			
		if(_wantsThrow)
		{
			_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
			_wantsThrow = false;
		}
		
		
		if(delta > RN::k::EpsilonFloat)
		{
			RN::Quaternion startRotation = GetWorldRotation();
			if(_targetRotation.GetDotProduct(startRotation) > 0.0f)
				startRotation = startRotation.GetConjugated();
			RN::Quaternion rotationSpeed = _targetRotation * startRotation;
			RN::Vector4 axisAngleSpeed = rotationSpeed.GetAxisAngle();
			if(axisAngleSpeed.w > 180.0f)
				axisAngleSpeed.w -= 360.0f;
			RN::Vector3 angularVelocity(axisAngleSpeed.x, axisAngleSpeed.y, axisAngleSpeed.z);
			angularVelocity *= axisAngleSpeed.w*M_PI;
			angularVelocity /= 180.0f;
			angularVelocity /= delta;
			_physicsBody->SetAngularVelocity(angularVelocity * 0.1f);
		}
	}

	void Stevelet::SetTargetPosition(RN::Vector3 position)
	{
		_targetPosition = position;
		_isMoving = true;
		
		RN::Vector3 direction = _targetPosition - GetWorldPosition();
		direction.y = 0.0f;
		direction.Normalize();
		
		_targetRotation = RN::Quaternion::WithLookAt(-direction, RN::Vector3(0.0f, 1.0f, 0.0f), false);
	}

	void Stevelet::SetTargetRotation(RN::Quaternion rotation)
	{
		_targetRotation = rotation;
	}

	void Stevelet::SetSteveletStats(const SteveStats &stats)
	{
		_stats = stats;
		SetTexture(_stats.GetSteveletFileName());
	}

	SteveStats const& Stevelet::GetSteveletStats() {
		return _stats;
	}

	void Stevelet::SetTexture(RN::String *file)
	{
		RN::Material *material = _model->GetLODStage(0)->GetMaterialAtIndex(0);
		material->RemoveAllTextures();
		material->AddTexture(RN::Texture::WithName(file));
	}
}
