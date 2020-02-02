//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_ANIMATABLE_H_
#define __ProjectSteve_ANIMATABLE_H_

#include "PSGrabbable.h"
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace PS
{
	class World;
	class Animatable : public Grabbable
	{
	public:
		Animatable(RN::String const* spriteName);
		~Animatable() = default;
		
		void Update(float delta) override;
		
		bool isAnimated = true;
		
	protected:
		RN::Model* _model;
		
		float _animationTimer;
		
		RNDeclareMeta(Animatable)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
