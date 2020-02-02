//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_EFFECTS_H_
#define __ProjectSteve_EFFECTS_H_

#include "PSSteveStats.h"
#include "PSEffect.h"

namespace PS
{
	class Obstacle;
	class Stevelet;
	class NoEffect : public Effect {
	public:
		void executeChallenge(Stevelet* steve) override {};
	};

	class BurnEffect : public Effect {
	public:
		BurnEffect(int _intensity = 5);
		void executeChallenge(Stevelet* steve) override;
	private:
		int intensity;
	};
	class SlowEffect : public Effect {
	public:
		void executeChallenge(Stevelet* steve) override;
	};
	class MilkEffect : public Effect {
	public:
		void executeChallenge(Stevelet* steve) override;
	};
	class FightEffect : public Effect {
	public:
		void executeChallenge(Stevelet* steve) override;
	};
	class PitEffect : public Effect {
	public:
		void executeChallenge(Stevelet* steve) override;
	};
	class WallEffect : public Effect {
	public:
		void executeChallenge(Stevelet* steve) override;
	};
	class FinishEffect : public Effect {
	public:
		FinishEffect(std::vector<Obstacle*> requirement, Obstacle* finish) : _req { requirement }, _finish{ finish } {};
		void executeChallenge(Stevelet* steve) override;
	private:
		std::vector<Obstacle*> _req;
		Obstacle* _finish;
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
