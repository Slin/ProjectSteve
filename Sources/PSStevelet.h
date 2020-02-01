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
		
		void SetTargetPosition(RN::Vector3 position);
		
	private:
		float _animationTimer;
		SteveStats _stats;
		RN::Vector3 _targetPosition;
		bool _isMoving;
		
		RN::PhysXDynamicBody *_physicsBody;
		
		RNDeclareMeta(Stevelet)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
