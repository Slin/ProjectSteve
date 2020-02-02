//
//  PSHelix.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSHelix.h"
#include "PSWorld.h"
#include "PSSpawner.h"

namespace PS
{
	const std::array<RN::Color, 4> GENE_COLORS = { {
		{0.5f, 0.5f, 0.5f},
		{0.2f, 0.4f, 0.84f},
		{0.77f, 0.23f, 0.16f},
		{0.92f, 0.38f, 0.f}
	} };

	static std::array<RN::Model*, 4> geneModels;


	RNDefineMeta(Helix, RN::Entity)
	
	Helix::Helix(World& _world)
	{
		//RN::ShaderLibrary *shaderLibrary = World::GetSharedInstance()->GetShaderLibrary();
		
		RN::Model *model = RN::Model::WithName(RNCSTR("models/dna.sgm"))->Copy();
		SetModel(model);
		
		geneModels[0] = RN::Model::WithName(RNCSTR("models/dna_blubb.sgm"));
		geneModels[0]->GetLODStage(0)->GetMaterialAtIndex(0)->SetDiffuseColor(GENE_COLORS[0]);
		for (size_t i = 1; i < geneModels.size(); ++i)
		{
			geneModels[i] = geneModels[0]->Copy();
			geneModels[i]->GetLODStage(0)->ReplaceMaterial(geneModels[1]->GetLODStage(0)->GetMaterialAtIndex(0)->Copy(), 0);
			geneModels[i]->GetLODStage(0)->GetMaterialAtIndex(0)->SetDiffuseColor(GENE_COLORS[i]);
		}

		float currentHeight = 0.04f;
		for(int i = 0; i < 12; i++)
		{
			/*_genes[i] = new Gene(static_cast<Gene::Type>(i % 4));*/
			if(i == 6) _genes[i] = new Gene(static_cast<Gene::Type>(i % 4));
			else _genes[i] = new Gene(Gene::Type::C);
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
		const Gene::Type t = gene.GetType();
		Gene& newGene = *new Gene(t);
		newGene.SetRotation(gene.GetWorldRotation());
		newGene.SetWorldPosition(gene.GetWorldPosition());
		world->AddLevelNode(newGene.Autorelease(), true);
		newGene.EnablePhysics();

		return &newGene;
	}

	void Helix::PlaceGene(Gene& target, Gene& newGene)
	{
		target.RemoveFlags(RN::SceneNode::Flags::Hidden);
		target.SetType(newGene.GetType());
		target.DisablePhysics();
		if(target.GetSpawner())
		{
			target.GetSpawner()->ReturnToPool(&target);
		}
		else
		{
			World* world = World::GetSharedInstance();
			world->RemoveLevelNode(&newGene);
		}
	}

	RNDefineMeta(Gene, Grabbable)

	Gene::Gene(Type type)
		: Grabbable(geneModels[static_cast<size_t>(type)]),
		_physicsBody(nullptr), 
		_type(type)
	{
		_spawner = nullptr;
	}

	void Gene::EnablePhysics()
	{
		if(_physicsBody) return;
		
		RN::PhysXMaterial *material = new RN::PhysXMaterial();
		RN::PhysXShape *shape = RN::PhysXBoxShape::WithHalfExtents(RN::Vector3(0.08f, 0.01f, 0.01f), material->Autorelease());
		_physicsBody = RN::PhysXDynamicBody::WithShape(shape, 0.1f);
		_physicsBody->SetCollisionFilter(World::CollisionType::Players, World::CollisionType::All);
		AddAttachment(_physicsBody);
	}

	void Gene::DisablePhysics()
	{
		if(!_physicsBody) return;
		
		RemoveAttachment(_physicsBody);
		_physicsBody = nullptr;
	}

	void Gene::Update(float delta)
	{
		Grabbable::Update(delta);
		
		if(_physicsBody)
		{
			_physicsBody->SetEnableKinematic(_isGrabbed);
			
			if(_wantsThrow)
			{
				_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
				_wantsThrow = false;
			}
		}
	}

	void Gene::SetType(Type type)
	{
		_type = type;
		this->SetModel(geneModels[static_cast<size_t>(type)]);
	}
}
