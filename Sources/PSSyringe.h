//
//  PSSyringe.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_SYRINGE_H_
#define __ProjectSteve_SYRINGE_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>
#include "PSAnimatable.h"
#include "PSSteveStats.h"

namespace PS
{
	class World;
	class Syringe : public Animatable
	{
	public:
		Syringe();
		~Syringe() = default;
		
		void Update(float delta) override;
		
		void SetPercentage(float percentage);
		void Trigger();
		void Reset() override;
		
	private:
		RN::PhysXDynamicBody *_physicsBody;
		RN::Entity *_fluidEntity;
		RN::Entity *_pumpEntity;
		
		float _fillPercentage;
		bool _isAnimating;
		
		RNDeclareMeta(Syringe)
	};
}

#endif /* defined(__ProjectSteve_SYRINGE_H_) */
