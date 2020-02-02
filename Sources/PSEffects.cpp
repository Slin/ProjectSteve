//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSEffects.h"
#include "PSWorld.h"

using Attr = PS::SteveStats::Attributes;
namespace PS
{
	BurnEffect::BurnEffect(int _intensity) : intensity{ _intensity } {};

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
		// Speed, Dexterity, -Gr��e		~(3, 4, -4) = 3
		SteveStats const& stats = steve->GetSteveletStats();

		steve->SetVelocity(log10(std::max(stats[Attr::SPEED] / 2 + stats[Attr::DEXTERITY] / 2 - stats[Attr::SIZE] * 2 + stats[Attr::WEIGHT], 1)));
	}

	void MilkEffect::executeChallenge(Stevelet* steve) {
		// Lactose lul
	}

	void FightEffect::executeChallenge(Stevelet* steve) {
		// Aggro, Shell, Strength		~(4, /, 4) = 8
		// Speed, Dexterity				~(3, 4)    = 7
	}

	void PitEffect::executeChallenge(Stevelet *steve)
	{
		SteveStats const& stats = steve->GetSteveletStats();
		if(stats[Attr::FLYING])
		{
			steve->SetFlying(0.25f, 0.5f);
		}
		else
		{
			steve->Jump(
				std::max(std::min(stats[Attr::SPEED] / 2 + stats[Attr::DEXTERITY] + stats[Attr::STRENGTH] + stats[Attr::BOUNCE], 11), 0) / 12.0f,
				0.4f * stats[Attr::SPEED] / 2);
		} 
	}

	void WallEffect::executeChallenge(Stevelet *steve)
	{
		SteveStats const& stats = steve->GetSteveletStats();
		if(stats[Attr::FLYING])
		{
			steve->SetFlying(0.25f, 0.5f);
		}
		else if (stats[Attr::CLIMB]) {
			steve->SetClimbing((stats[Attr::STRENGTH] - stats[Attr::WEIGHT] + stats[Attr::DEXTERITY] / 2) / 2.0f);
		}
		else
		{
			steve->Jump(
				std::max(std::min(stats[Attr::SPEED] / 2 + stats[Attr::DEXTERITY] / 2 + stats[Attr::STRENGTH] / 2 + stats[Attr::BOUNCE], 11), 0) / 6.0f);
		}
	}


	void FinishEffect::executeChallenge(Stevelet *steve) {
		
		bool finished = true;
		for(auto& e : _req) {
			if (!steve->DidFinish(e)) finished = false;
		}
		_finish->FreeStevelet(steve);

		if (finished) World::GetSharedInstance()->TriggerSteveletWin(steve);
	}

}
