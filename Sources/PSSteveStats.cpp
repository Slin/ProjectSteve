//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include <Rayne.h>
#include "PSSteveStats.h"

std::array<std::array<int, 4> 11> GenPermutations = {{
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
	{0, 1, 3, 2}}};

int decodeGenes(const DNA& dna, const std::array<int, 3>& ids, const std::array<int, 4>& permutation) {
	int value = 0;
	for(const auto& id : ids) {
		value += permutation[static_cast<int>(dna[id].GetType())];
	}
	return value;
}

template<std::size_t N>
std::string numbersToString(const std::array<int, N>& numbers) {
	std::string str = "";
	for(int i : numbers) {
		str += std::to_string(i);
	}
	return str;
}

namespace PS
{
	SteveStates::SteveStates(const DNA& dna) {
		SteveStates& self = *this;
		self[STRENGTH] =  decodeGenes(dna, {1, 2, 3}, GenPermutations[0]);
		self[SIZE] =  decodeGenes(dna, {6, 8, 4}, GenPermutations[1]);
		self[INTELLIGENCE] =  decodeGenes(dna, {2, 3, 4}, GenPermutations[2]);
		self[FAT_CONTENT] =  decodeGenes(dna, {5, 6, 7}, GenPermutations[3]);
		self[AGGRO] =  decodeGenes(dna, {7, 8, 9}, GenPermutations[4]);
		self[DEXTERITY] =  decodeGenes(dna, {6, 8, 2}, GenPermutations[5]);
		self[WEIGHT] =  (self[STRENGTH] + self[SIZE] + self[FAT_CONTENT])  / 3.f;
		
		self[WINGS] = decodeGenes(dna, {11, 10, 8}, GenPermutations[6]) > 7 ? 2 : 0;
		self[SHELL] = decodeGenes(dna, {10, 11, 4}, GenPermutations[7]) > 6 ? 2 : 0;
		self[HANDS] = decodeGenes(dna, {11, 10, 7}, GenPermutations[8]) > 5 ? 2 : 0;
		self[LEGS] = decodeGenes(dna, {11, 10, 7}), GenPermutations[9]) > 5 ? 2 : 0;
		self[LACTRASE] = decodeGenes(dna, {10, 11, 9}, GenPermutations[10]) > 8 ? 2 : 0;
		
		self[SPEED] =  ((self[STRENGTH] - self[WEIGHT]) + self[DEXTERITY]) / 2.f + self[LEGS];

		self[FLYING] = (self[WINGS] && self[STRENGTH] > self[WEIGHT]) ? 1 : 0;
		self[CLIMB] = (self[HANDS] && self[STRENGTH]  > self[FAT_CONTENT]) ? 1 : 0;
		self[BOUNCE] = (self[FAT_CONTENT] > self[SIZE]) ? 1 : 0;
	}
	
	RN::String *SteveStats::GetSteveletFileName()
	{
		SteveStates& self = *this;
		std::array<int, 6> phenoType = {{
		std::min(3, (self[SHELL] + self[DEXTERITY] + 2) / 5), // armor 3
		self[HANDS]  ? 1 : 0, // arms 1
		(self[AGGRO]  + 1) / 3, // evil 3
		self[LEGS] ? 1 : 0, // feet 1
		(self[INT]  + 1) / 3, // int 3
		std::min(3, (self[STRENGTH + SIZE] + 2) / 5), // testosterone 3
		self[WINGS] ? 1 : 0// wings 1
		}}
		return RNCSTR("sprites/stevelet/" +  numbersToString(phenoType) + ".png");
	}
}
