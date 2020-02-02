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
	
	Stevelet::Stevelet() : Animatable(RNCSTR("sprites/stevelet/0000000.png")), _isMoving(false), _flightTime(0.0f), _flightHeight(0.0f)
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

		RNDebug("kill");
		_spawner->ReturnToPool(this);
	}

	void Stevelet::Vanish() {
		SetWorldPosition({ 0, 0, -42 });
	}

	void Stevelet::Jump(float intensity, float velocity) {
		_physicsBody->ApplyForce({ 0, intensity, 0 });
		SetVelocity(velocity);
	}

	void Stevelet::SetFlying(float height, float distance)
	{
		Jump(height);
		_flightTime = distance/_targetVelocity;
		_flightHeight = height;
	}
	
	void Stevelet::SetClimbing(float duration) {
		_climbTime = duration;
		
		_climbZ = GetForward().z > GetForward().x ? GetWorldPosition().z : GetWorldPosition().x;
	}

	void Stevelet::ResetVelocity() {
		_targetVelocity = _defaultVelocity;
	}

	void Stevelet::SetVelocity(float velocity) {
		_targetVelocity = velocity;
	}

	void Stevelet::EnterObstacleCourse(RN::Vector3 position)
	{
		_physicsBody->SetLinearVelocity(RN::Vector3());
		ResetVelocity();
		SetTargetPosition(position);
			//RN::Vector3(GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z + 1.0f));
		
		RNDebug("Enter ObstacleBlubb");
	}

	void Stevelet::LeaveObstacleCourse()
	{
		float offset = RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(1.3f, 2.4f);
		SetTargetPosition(RN::Vector3(GetWorldPosition().x - offset, GetWorldPosition().y, GetWorldPosition().z));
		
		RNDebug("Leave ObstacleBlubb");
	}

	void Stevelet::Update(float delta)
	{
		delta = std::min(0.05f, delta);
		Animatable::Update(delta);
		
		if(_flightTime > 0.0f)
		{
			_physicsBody->ApplyForce({0.0f, 9.81f*0.4f*delta, 0.0f});
		}
		_flightTime -= delta;

		if (_climbTime > 0.0f) {
			if(_physicsBody->GetLinearVelocity().y < 1.0f) _physicsBody->ApplyForce({ 0.0f, 13.0f * 0.5f * delta, 0.0f });
			_climbTime -= delta;

			float newPos = GetForward().z > GetForward().x ? GetWorldPosition().z : GetWorldPosition().x;
			if ((newPos - _climbZ) > 0.1f) {
				_climbTime = 0.0f;
				_physicsBody->ClearForces();
			}
		}
		
		RN::Vector3 direction = _targetPosition - GetWorldPosition();
		direction.y = 0.0f;
		if(direction.GetLength() > 0.2f)
		{
			if(_isMoving)
			{
				RN::Vector3 linearVelocity = _physicsBody->GetLinearVelocity();
				linearVelocity.y = 0.0f;
				if(linearVelocity.GetLength() < _targetVelocity)
					_physicsBody->ApplyForce(direction.Normalize() * 0.08f);
			}
		}
		else
		{
			_isMoving = false;
		}
		
		if(!_isMoving)
		{
			SetTargetPosition(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomVector3Range(RN::Vector3(-1.0f, 0.15f, -1.7f), RN::Vector3(1.7f, 0.15f, 1.0f)));
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
		//	RNDebug(angularVelocity.GetLength());
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
		const size_t hash = file->GetHash();
		if (hash == _textureHash) return;

		_textureHash = hash;
		RN::Material *material = _model->GetLODStage(0)->GetMaterialAtIndex(0);
		material->RemoveAllTextures();
		material->AddTexture(RN::Texture::WithName(file));
	}
}
