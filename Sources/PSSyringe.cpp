//
//  PSSyringe.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSSyringe.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Syringe, Animatable)
	
	Syringe::Syringe() : Animatable(RNCSTR("models/syringe_body.sgm")), _fillPercentage(0.0f), _isAnimating(false)
	{
		RN::PhysXMaterial *material = new RN::PhysXMaterial();
		RN::PhysXShape *shape = RN::PhysXBoxShape::WithHalfExtents(RN::Vector3(0.08f, 0.01f, 0.015f), material);
		_physicsBody = RN::PhysXDynamicBody::WithShape(shape, 0.1f);
		_physicsBody->SetCollisionFilter(World::CollisionType::Players, World::CollisionType::All);
		AddAttachment(_physicsBody);
		
		_fluidEntity = new RN::Entity(RN::Model::WithName(RNCSTR("models/syringe_fluid.sgm")));
		_pumpEntity = new RN::Entity(RN::Model::WithName(RNCSTR("models/syringe_pump.sgm")));
		
		AddChild(_pumpEntity->Autorelease());
		AddChild(_fluidEntity->Autorelease());
	}

	void Syringe::Update(float delta)
	{
		Animatable::Update(delta);
		
		if(_isGrabbed)
		{
			_physicsBody->SetLinearVelocity(RN::Vector3());
			
			if(_isTriggered)
			{
				Trigger();
			}
		}
		
		if(_wantsThrow)
		{
			_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
			_wantsThrow = false;
		}
		
		if(_isAnimating)
		{
			_fillPercentage += delta;
			if(_fillPercentage > 1.0f) _fillPercentage = 1.0f;
			SetPercentage(_fillPercentage);
		}
	}

	void Syringe::SetPercentage(float percentage)
	{
		float distance = 0.15f * percentage;
		_pumpEntity->SetPosition(RN::Vector3(-distance, 0.0f, 0.0f));
		_fluidEntity->SetPosition(RN::Vector3(-distance*0.5f, 0.0f, 0.0f));
		_fluidEntity->SetScale(RN::Vector3(1.0f-percentage, 1.0f, 1.0f));
	}

	void Syringe::Reset()
	{
		_fillPercentage = 0.0f;
		_isAnimating = false;
		SetPercentage(_fillPercentage);
		
		Grabbable::Reset();
	}

	void Syringe::Trigger()
	{
		_isAnimating = true;
	}
}
