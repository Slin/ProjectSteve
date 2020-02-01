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

namespace PS
{
	class World;
	class Stevelet : public RN::Entity
	{
	public:
		Stevelet();
		~Stevelet() = default;
		
		void Update(float delta) override;

	private:
		float _animationTimer;
		
		RNDeclareMeta(Stevelet)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
