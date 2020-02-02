//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include <Rayne.h>
#include "PSSteveStats.h"

namespace PS {

	std::array<std::array<int, 4>, 11> GenPermutations = { {
		{3,0,2,1},
		{0,1,3,2},
		{2,0,3,1},
		{3,0,2,1},
		{0,1,3,2},
		{1,0,3,2},
		{1,2,3,0},
		{3,1,0,2},
		{1,2,3,0},
		{2,0,1,3},
		{2,0,3,1}} };

	int decodeGenes(const DNA& dna, const std::array<int, 3>& ids, const std::array<int, 4>& permutation) {
		int value = 0;
		for (const auto& id : ids) {
			value += permutation[static_cast<int>(dna[id]->GetType())];
		}
		return value;
	}

	template<std::size_t N>
	std::string numbersToString(const std::array<int, N>& numbers) {
		std::string str = "";
		for (int i : numbers) {
			str += std::to_string(i);
		}
		return str;
	}

	using Attr = SteveStats::Attributes;

	SteveStats::SteveStats(const DNA& dna) {
		SteveStats& self = *this;

		self[Attr::STRENGTH] = decodeGenes(dna, { 1, 2, 3 }, GenPermutations[0]);
		self[Attr::SIZE] = decodeGenes(dna, { 6, 8, 4 }, GenPermutations[1]);
		self[Attr::INTELLIGENCE] = decodeGenes(dna, { 2, 5, 4 }, GenPermutations[2]);
		self[Attr::FAT_CONTENT] = decodeGenes(dna, { 5, 6, 7 }, GenPermutations[3]);
		self[Attr::AGGRO] = decodeGenes(dna, { 7, 8, 9 }, GenPermutations[4]);
		self[Attr::DEXTERITY] = decodeGenes(dna, { 6, 8, 2 }, GenPermutations[5]);
		self[Attr::WEIGHT] = (self[Attr::STRENGTH] + self[Attr::SIZE] + self[Attr::FAT_CONTENT]) / 3.f;

		self[Attr::WINGS] = decodeGenes(dna, { 11, 10, 8 }, GenPermutations[6]) > 6 ? 2 : 0;
		self[Attr::SHELL] = decodeGenes(dna, { 10, 11, 4 }, GenPermutations[7]) > 4 ? 2 : 0;
		self[Attr::HANDS] = decodeGenes(dna, { 11, 10, 6 }, GenPermutations[8]) > 4 ? 2 : 0;
		self[Attr::LEGS] = decodeGenes(dna, { 11, 10, 7 }, GenPermutations[9]) > 4 ? 2 : 0;
		self[Attr::LACTRASE] = decodeGenes(dna, { 10, 11, 5 }, GenPermutations[10]) > 8 ? 2 : 0;

		self[Attr::SPEED] = ((self[Attr::STRENGTH] - self[Attr::WEIGHT]) + self[Attr::DEXTERITY]) / 2.f + self[Attr::LEGS];

		self[Attr::FLYING] = (self[Attr::WINGS] && self[Attr::STRENGTH] > self[Attr::WEIGHT]) ? 1 : 0;
		self[Attr::CLIMB] = (self[Attr::HANDS] && self[Attr::STRENGTH] > self[Attr::FAT_CONTENT]) ? 1 : 0;
		self[Attr::BOUNCE] = (self[Attr::FAT_CONTENT] > self[Attr::SIZE]) ? 1 : 0;
	}
	
	int mapV(int i) {
		if (i < 4) return 0;
		if (i < 5) return 1;
		if (i < 6) return 2;
		return 3;
	}

	RN::String* SteveStats::GetSteveletFileName()
	{
		SteveStats& self = *this;
		std::array<int, 7> phenoType = { {
			mapV(static_cast<int>((self[Attr::SHELL] + self[Attr::DEXTERITY]) / 13.f * 9.f)), // armor 3
			self[Attr::HANDS] ? 1 : 0, // arms 1
			mapV(self[Attr::AGGRO]), // evil 3
			self[Attr::LEGS] ? 1 : 0, // feet 1
			mapV(self[Attr::INTELLIGENCE]), // int 3
			mapV(self[Attr::STRENGTH]), // testosterone 3
			self[Attr::WINGS] ? 1 : 0// wings 1
		} };
		return RNSTR("sprites/stevelet/" << numbersToString(phenoType) << ".png");
	}
	
	std::array<int, static_cast<int>(SteveStats::Attributes::COUNT)> StatsKnowledge::s_statsKnown = {};

	void StatsKnowledge::Discover(const SteveStats& stats)
	{
		for (int i = 0; i < 12; ++i)
		{
			const Attr attr = static_cast<Attr>(i);
			s_statsKnown[i] = std::max(s_statsKnown[i], stats[attr]);
		}
	}

	int StatsKnowledge::IsKnown(SteveStats::Attributes attr)
	{
		return s_statsKnown[static_cast<size_t>(attr)];
	}

}
