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
		{0, 1, 2, 3},
		{1, 3, 2, 0},
		{3, 2, 1, 0},
		{2, 1, 3, 0},
		{1, 3, 0, 2},
		{4, 2, 1, 0},
		{0, 2, 3, 1},
		{0, 3, 2, 1},
		{2, 3, 1, 0},
		{3, 0, 2, 1},
		{0, 1, 3, 2}} };

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
		self[Attr::INTELLIGENCE] = decodeGenes(dna, { 2, 3, 4 }, GenPermutations[2]);
		self[Attr::FAT_CONTENT] = decodeGenes(dna, { 5, 6, 7 }, GenPermutations[3]);
		self[Attr::AGGRO] = decodeGenes(dna, { 7, 8, 9 }, GenPermutations[4]);
		self[Attr::DEXTERITY] = decodeGenes(dna, { 6, 8, 2 }, GenPermutations[5]);
		self[Attr::WEIGHT] = (self[Attr::STRENGTH] + self[Attr::SIZE] + self[Attr::FAT_CONTENT]) / 3.f;

		self[Attr::WINGS] = decodeGenes(dna, { 11, 10, 8 }, GenPermutations[6]) > 7 ? 2 : 0;
		self[Attr::SHELL] = decodeGenes(dna, { 10, 11, 4 }, GenPermutations[7]) > 6 ? 2 : 0;
		self[Attr::HANDS] = decodeGenes(dna, { 11, 10, 7 }, GenPermutations[8]) > 5 ? 2 : 0;
		self[Attr::LEGS] = decodeGenes(dna, { 11, 10, 7 }, GenPermutations[9]) > 5 ? 2 : 0;
		self[Attr::LACTRASE] = decodeGenes(dna, { 10, 11, 9 }, GenPermutations[10]) > 8 ? 2 : 0;

		self[Attr::SPEED] = ((self[Attr::STRENGTH] - self[Attr::WEIGHT]) + self[Attr::DEXTERITY]) / 2.f + self[Attr::LEGS];

		self[Attr::FLYING] = (self[Attr::WINGS] && self[Attr::STRENGTH] > self[Attr::WEIGHT]) ? 1 : 0;
		self[Attr::CLIMB] = (self[Attr::HANDS] && self[Attr::STRENGTH] > self[Attr::FAT_CONTENT]) ? 1 : 0;
		self[Attr::BOUNCE] = (self[Attr::FAT_CONTENT] > self[Attr::SIZE]) ? 1 : 0;
	}

	RN::String* SteveStats::GetSteveletFileName()
	{
		SteveStats& self = *this;
		std::array<int, 7> phenoType = { {
		std::min(3, (
			self[Attr::SHELL] + self[Attr::DEXTERITY] + 2) / 5), // armor 3
			self[Attr::HANDS] ? 1 : 0, // arms 1
			(self[Attr::AGGRO] + 1) / 3, // evil 3
			self[Attr::LEGS] ? 1 : 0, // feet 1
			(self[Attr::INTELLIGENCE] + 1) / 3, // int 3
			std::min(3, (self[Attr::STRENGTH] + self[Attr::SIZE] + 2) / 5), // testosterone 3
			self[Attr::WINGS] ? 1 : 0// wings 1
		} };
		return RNSTR("sprites/stevelet/" << numbersToString(phenoType) << ".png");
	}
	


}