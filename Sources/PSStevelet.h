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
		void MoveForward();
		void StopMovement();
		void FreezePhysics();
		void ResumePhysics();
		
		void SetTargetPosition(RN::Vector3 position);
		void SetTargetRotation(RN::Vector3 rotation);
		
		void SetSteveletStats(const SteveStats &stats);
		
	private:
		SteveStats _stats;
		RN::Vector3 _targetPosition;
		RN::Quaternion _targetRotation;
		bool _isMoving;
		bool _isWalking = false;
		
		void SetTexture(RN::String *file);

		float const _targetVelocity = 0.4f;
		
		RN::PhysXDynamicBody *_physicsBody;
		bool _physicsEnabled = true;
		
		RNDeclareMeta(Stevelet)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
