//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSStevelet.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Stevelet, Animatable)
	
	Stevelet::Stevelet() : Animatable(RNCSTR("sprites/Pyxel2DBlob_Arms_Idle.png")), _animationTimer(0.0f)
	{
		
	}

	void Stevelet::Update(float delta) {
		Animatable::Update(delta);

		Translate(GetForward() * delta * 0.1f);
	}
}
