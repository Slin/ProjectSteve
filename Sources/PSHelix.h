//
//  PSHelix.h
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_HELIX_H_
#define __ProjectSteve_HELIX_H_

#include "PSAnimatable.h"
#include "PSGrabbable.h"
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace PS
{
	class World;
	class Gene;

	class Helix : public RN::Entity
	{
	public:
		Helix(World& _world);
		~Helix() = default;
		
		void Update(float delta) override;

		// @return copy of the gene to move around or nullptr if the given gene is already missing
		Gene* PickGene(Gene& gene);
		void PlaceGene(Gene& target, Gene& newGene);
	private:
		std::array<Gene*, 12> _genes;
		RN::Model* _geneModel;
		RNDeclareMeta(Helix)
	};

	class Gene : public Grabbable
	{
	public:
		using Grabbable::Grabbable;
	private:
		RNDeclareMeta(Gene)
	};
}

#endif /* defined(__ProjectSteve_HELIX_H_) */
