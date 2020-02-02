//
//  PSPlayer.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_EFFECT_H_
#define __ProjectSteve_EFFECT_H_


namespace PS
{
	class Stevelet;
	class Obstacle;
	class Effect{
	public:
		virtual void executeChallenge(Stevelet* steve, Obstacle *obstacle) = 0;
	private:
	};
}

#endif /* defined(__ProjectSteve_STEVELET_H_) */
