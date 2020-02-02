//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSEffects.h"

using Attr = PS::SteveStats::Attributes;
namespace PS
{
	BurnEffect::BurnEffect(int _intensity = 5) : intensity{ _intensity } {};

	void BurnEffect::executeChallenge(Stevelet* steve) {
		// Speed, KFA, Shell			~(4, 3, /) = 7
		// Speed, Dexterity, Strength	~(3, 4, 4) = 11
		SteveStats const& stats = steve->GetSteveletStats();
		
		if (stats[Attr::SPEED] / 2 + stats[Attr::FAT_CONTENT] / 2 + stats[Attr::SHELL] > intensity) {
			steve->SetVelocity(1.5f);
		} else if (stats[Attr::SPEED] / 2 + stats[Attr::DEXTERITY] + stats[Attr::STRENGTH] + stats[Attr::BOUNCE]) {
			steve->Jump(1.0f);
		} else {
			steve->Vanish();
		}
	}
	
	void SlowEffect::executeChallenge(Stevelet* steve) {
		// Speed, Dexterity, -Größe		~(3, 4, -4) = 3
	}

	void MilkEffect::executeChallenge(Stevelet* steve) {
		// Lactose lul
	}

	void FightEffect::executeChallenge(Stevelet* steve) {
		// Aggro, Shell, Strength		~(4, /, 4) = 8
		// Speed, Dexterity				~(3, 4)    = 7
	}
}
