//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_OBSTACLE_H_
#define __ProjectSteve_OBSTACLE_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>
#include "PSAnimatable.h"
#include "PSStevelet.h"
#include "PSEffects.h"

namespace PS
{
	class World;
	class Level;
	class Obstacle : public RN::Entity
	{
	public:
		Obstacle(RN::String const* modelName, const RN::String *collisionModelName, Level* parent);
		~Obstacle();
		void SetEffect(Effect* effect);
		void SetZOriented(bool zOriented);
		void AssignStevelet(Stevelet* steve);
		bool Contains(Stevelet* steve);
		void Update(float delta) override;
		bool IsReached(float z);
		float GetZTreshold();
	private:
		std::vector<Stevelet*> _steves;
		Level* _parent;
		Effect* _effect = new NoEffect();
		bool _isZOriented = true;
		
		RNDeclareMeta(Obstacle)
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
