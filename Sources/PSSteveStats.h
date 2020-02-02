//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_STEVESTATS_H_
#define __ProjectSteve_STEVESTATS_H_

#include <Rayne.h>
#include "PSHelix.h"

namespace PS
{
	class SteveStats
	{
	public:
		SteveStats() = default;
		SteveStats(const DNA& dna);
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
		int operator[](Attributes index) const { return _attr[static_cast<int>(index)]; }
		int& operator[](Attributes index) { return _attr[static_cast<int>(index)]; }
		
		RN::String *GetSteveletFileName();
		
	private:
		std::array<int, static_cast<int>(Attributes::COUNT)> _attr = {};
	};

	using ReducedDNA = std::array<Gene::Type, 12>;

	class StatsKnowledge
	{
	public:
		static void Discover(const SteveStats& stats);
		static int IsKnown(SteveStats::Attributes attr);
	private:
		static std::array<int, static_cast<int>(SteveStats::Attributes::COUNT)> s_statsKnown;
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
