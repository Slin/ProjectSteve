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
		_physicsBody->SetCollisionFilter(World::CollisionType::Players, World::CollisionType::All);
		AddAttachment(_physicsBody);
	}

	void Stevelet::Update(float delta)
	{
		Animatable::Update(delta);
		
		RN::Vector3 direction = _targetPosition - GetWorldPosition();
		direction.y = 0.0f;
		if(direction.GetLength() > 0.2f)
		{
			if(_isMoving)
			{
				_physicsBody->ApplyForce(direction.Normalize() * 0.02f);
			}
		}
		else
		{
			_isMoving = false;
		}
		
		if(!_isMoving)
		{
			SetTargetPosition(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomVector3Range(RN::Vector3(-2.0f, 0.15f, -2.0f), RN::Vector3(2.0f, 0.15f, 2.0f)));
		}
		
		if(_isGrabbed)
		{
			_physicsBody->SetLinearVelocity(RN::Vector3());
		}
		
		if(_wantsThrow)
		{
			_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
			_wantsThrow = false;
		}
	}

	void Stevelet::SetTargetPosition(RN::Vector3 position)
	{
		_targetPosition = position;
		_isMoving = true;
	}
}
