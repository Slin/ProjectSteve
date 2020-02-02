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
		Level(bool zOriented = true) : _isZOriented{ zOriented } {};
		~Level() = default;
		void AddObstacle(Obstacle* obs);
		void AssignStevelet(Stevelet* steve);
		void FreeStevelet(Stevelet* steve, Obstacle* obs);
		void RemoveStevelet(Stevelet* steve);
		void Update(float delta) override;
	private:
		std::vector<Obstacle*> _obstacles;
		RN::Vector3 CalculateEndPosition();

		bool _isZOriented = true;

		std::vector<std::tuple<float, Stevelet*>> _winners;
		RN::AABB _startTrigger;

		RNDeclareMeta(Level)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
