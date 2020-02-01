//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSStevelet.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Stevelet, Animatable)
	
	Stevelet::Stevelet() : Animatable(RNCSTR("sprites/Pyxel2DBlob_Arms_Idle.png")), _isMoving(false)
	{
		RN::PhysXMaterial *material = new RN::PhysXMaterial();
		RN::PhysXShape *shape = RN::PhysXSphereShape::WithRadius(0.15, material);
		_physicsBody = RN::PhysXDynamicBody::WithShape(shape, 0.5f);
		_physicsBody->SetCollisionFilter(World::CollisionType::Players, World::CollisionType::Level);
		AddAttachment(_physicsBody);
	}

	void Stevelet::Kill() {
		//World::GetSharedInstance()->RemoveLevelNode(this);
		StopMovement();
	}

	void Stevelet::MoveForward() {
		_isWalking = true;
	}

	void Stevelet::StopMovement() {
		_isWalking = false;
		_physicsBody->SetLinearVelocity(RN::Vector3());
	}

	void Stevelet::FreezePhysics() {
		_physicsBody->ClearForces();
		_physicsEnabled = false;
	}

	void Stevelet::ResumePhysics() {
		StopMovement();
		_physicsEnabled = true;
		_isMoving = false;
	}

	void Stevelet::Update(float delta)
	{
		delta = std::min(0.05f, delta);
		Animatable::Update(delta);
		
		RN::Vector3 direction = _targetPosition - GetWorldPosition();
		direction.y = 0.0f;
		if(direction.GetLength() > 0.2f)
		{
			if(_isMoving && _physicsEnabled)
			{
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
		
		if(_isGrabbed && _physicsEnabled)
		{
			_physicsBody->SetLinearVelocity(RN::Vector3());
		}
		
		if(_wantsThrow && _physicsEnabled)
		{
			_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
			_wantsThrow = false;
		}
		
		if (delta > RN::k::EpsilonFloat) {
			RN::Quaternion startRotation = GetWorldRotation();
			if (_targetRotation.GetDotProduct(startRotation) > 0.0f)
				startRotation = startRotation.GetConjugated();
			RN::Quaternion rotationSpeed = _targetRotation * startRotation;
			RN::Vector4 axisAngleSpeed = rotationSpeed.GetAxisAngle();
			if (axisAngleSpeed.w > 180.0f)
				axisAngleSpeed.w -= 360.0f;
			RN::Vector3 angularVelocity(axisAngleSpeed.x, axisAngleSpeed.y, axisAngleSpeed.z);
			angularVelocity *= axisAngleSpeed.w*M_PI;
			angularVelocity /= 180.0f;
			angularVelocity /= delta;
			_physicsBody->SetAngularVelocity(angularVelocity);
		}

		if (_isWalking) {
			_physicsBody->SetLinearVelocity(RN::Vector3(0, 0, _targetVelocity));
		}
	}

	void Stevelet::SetTargetPosition(RN::Vector3 position)
	{
		_targetPosition = position;
		_isMoving = true;
	}

	void Stevelet::SetTargetRotation(RN::Vector3 rotation)
	{
		_targetRotation = rotation;
	}
}
