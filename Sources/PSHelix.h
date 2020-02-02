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

	class Gene : public Grabbable
	{
	public:
		enum struct Type
		{
			C,
			G,
			A,
			T
		};

		Gene(Type type);
		~Gene() = default;

		void Update(float delta) override;

		void EnablePhysics();
		void DisablePhysics();

		Type GetType() const { return _type; }
		void SetType(Type type);
	private:
		RN::PhysXDynamicBody* _physicsBody;
		Type _type;
		RNDeclareMeta(Gene)
	};
	using DNA = std::array<Gene*, 12>;
	class Helix : public RN::Entity
	{
	public:
		Helix(World& _world);
		~Helix() = default;
		
		void Update(float delta) override;

		// @return copy of the gene to move around or nullptr if the given gene is already missing
		Gene* PickGene(Gene& gene);
		void PlaceGene(Gene& target, Gene& newGene);
		
		const DNA &GetDNA() const { return _genes; }
		
	private:
		DNA _genes;
		RNDeclareMeta(Helix)
	};
}

#endif /* defined(__ProjectSteve_HELIX_H_) */
