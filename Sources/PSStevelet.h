//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_STEVELET_H_
#define __ProjectSteve_STEVELET_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>
#include "PSAnimatable.h"
#include "PSSteveStats.h"

namespace PS
{
	class World;
	class Stevelet : public Animatable
	{
	public:
		Stevelet();
		~Stevelet() = default;
		void Update(float delta) override;

		void Kill();
		void ResetVelocity();
		void SetVelocity(float velocity);
		void Jump(float intensity, float velocity = 0.4f);
		void SetFlying(float height, float distance);
		void SetClimbing(float duration);
		void Vanish();
		void EnterObstacleCourse(RN::Vector3 position);
		void LeaveObstacleCourse();
		
		void SetTargetPosition(RN::Vector3 position);
		void SetTargetRotation(RN::Quaternion rotation);
		
		void SetSteveletStats(const SteveStats &stats);
		SteveStats const& GetSteveletStats();
		
	private:
		SteveStats _stats;
		RN::Vector3 _targetPosition;
		RN::Quaternion _targetRotation;
		bool _isMoving;
		
		void SetTexture(RN::String *file);

		float const _defaultVelocity = 0.4f;
		float _targetVelocity = _defaultVelocity;
		
		float _flightTime;
		float _flightHeight;

		float _climbTime;
		float _climbZ;
		
		RN::PhysXDynamicBody *_physicsBody;
		
		RNDeclareMeta(Stevelet)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
