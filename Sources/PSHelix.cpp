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
		newGene.EnablePhysics();

		return &newGene;
	}

	void Helix::PlaceGene(Gene& target, Gene& newGene)
	{
		target.RemoveFlags(RN::SceneNode::Flags::Hidden);
		World* world = World::GetSharedInstance();
		world->RemoveLevelNode(&newGene);
	}

	RNDefineMeta(Gene, Grabbable)

	Gene::Gene(RN::Model *model) : Grabbable(model), _physicsBody(nullptr)
	{
		
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
			if(_isGrabbed)
			{
				_physicsBody->SetLinearVelocity(RN::Vector3());
			}
			
			if(_wantsThrow)
			{
				_physicsBody->SetLinearVelocity(_currentGrabbedSpeed);
				_wantsThrow = false;
			}
		}
	}
}
