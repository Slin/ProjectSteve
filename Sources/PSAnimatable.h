//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_ANIMATABLE_H_
#define __ProjectSteve_ANIMATABLE_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace PS
{
	class World;
	class Animatable : public RN::Entity
	{
	public:
		Animatable(RN::String const* spriteName);
		~Animatable() = default;
		
		void SetIsGrabbed(bool isGrabbed);
		
		void Update(float delta) override;
	protected:
		RN::Model* _model;
		bool _isGrabbed;
		bool _wantsThrow;
		
		RN::Vector3 _previousPosition;
		RN::Vector3 _currentGrabbedSpeed;
		RN::Vector3 _currentGrabbedRotationSpeed;
		
	private:
		float _animationTimer;
		
		RNDeclareMeta(Animatable)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
