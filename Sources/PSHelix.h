//
//  PSHelix.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_HELIX_H_
#define __ProjectSteve_HELIX_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace PS
{
	class World;
	class Helix : public RN::Entity
	{
	public:
		Helix();
		~Helix() = default;
		
		void Update(float delta) override;

	private:
		
		RNDeclareMeta(Helix)
	};
}

#endif /* defined(__ProjectSteve_HELIX_H_) */
