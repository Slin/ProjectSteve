//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_STEVESTATS_H_
#define __ProjectSteve_STEVESTATS_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>
#include "PSAnimatable.h"
#include "PSSteveStats.h"

namespace PS
{
	class SteveStats
	{
	public:
		SteveStats();
		~SteveStats() = default;
		enum struct Attributes {
			STRENGTH, 
			SIZE,
			INTELLIGENCE,
			FAT_CONTENT,
			AGGRO,
			DEXTERITY,
			SPEED,
			WEIGHT,
			WINGS,
			SHELL,
			HANDS,
			LEGS,
			LACTRASE,

			FLYING,
			CLIMB,
			BOUNCE,

			COUNT
		};
		const int& operator[](Attributes index) const { return _attr[static_cast<int>(index)]; }
		int& operator[](Attributes index) { return _attr[static_cast<int>(index)]; }
	private:
		std::array<int, static_cast<int>(Attributes::COUNT)> _attr;
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
