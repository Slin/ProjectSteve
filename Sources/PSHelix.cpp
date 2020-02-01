//
//  PSHelix.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSHelix.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Helix, RN::Entity)
	
	Helix::Helix(World& _world)
	{
		//RN::ShaderLibrary *shaderLibrary = World::GetSharedInstance()->GetShaderLibrary();
		
		RN::Model *model = RN::Model::WithName(RNCSTR("models/dna.sgm"))->Copy();
		SetModel(model);
		
		_geneModel = RN::Model::WithName(RNCSTR("models/dna_blubb.sgm"));
		float currentHeight = 0.04f;
		for(int i = 0; i < 12; i++)
		{
			_genes[i] = new Gene(_geneModel);
			AddChild(_genes[i]);
			_world.RegisterGrabbable(_genes[i]);
			_genes[i]->SetPosition(RN::Vector3(0.0f, currentHeight, 0.0f));
			_genes[i]->SetRotation(RN::Vector3(200.0f/(0.08*12.0f) * currentHeight, 0.0f, 0.0f));
			
			currentHeight += 0.08f;
		}
	}

	void Helix::Update(float deltaTime)
	{
		Rotate(RN::Vector3(deltaTime * 16.0f, 0.0f, 0.0f));
	}

	Gene* Helix::PickGene(Gene& gene)
	{
		if (gene.GetFlags() && RN::SceneNode::Flags::Hidden)
			return nullptr;

		gene.AddFlags(RN::SceneNode::Flags::Hidden);
		World* world = World::GetSharedInstance();
		Gene& newGene = *new Gene(_geneModel);
		newGene.SetRotation(gene.GetWorldRotation());
		newGene.SetWorldPosition(gene.GetWorldPosition());
		world->AddLevelNode(newGene.Autorelease(), true);

		return &newGene;
	}

	void Helix::PlaceGene(Gene& target, Gene& newGene)
	{
		target.RemoveFlags(RN::SceneNode::Flags::Hidden);
		World* world = World::GetSharedInstance();
		world->RemoveLevelNode(&newGene);
	}

	RNDefineMeta(Gene, RN::Entity)
}
