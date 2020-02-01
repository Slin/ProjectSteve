//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_EXPERIMENTLEVEL_H_
#define __ProjectSteve_EXPERIMENTLEVEL_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>
#include "PSAnimatable.h"
#include "PSStevelet.h"
#include "PSObstacle.h"

namespace PS
{
	class World;
	class Level : public RN::Entity
	{
	public:
		Level() = default;
		~Level() = default;
		void AddObstacle(Obstacle* obs);
		void FreeSteve(Stevelet* steve, Obstacle* obs);
	private:
		std::vector<Obstacle*> _obstacles;
		RNDeclareMeta(Level)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
