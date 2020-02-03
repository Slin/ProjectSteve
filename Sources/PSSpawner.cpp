//
//  PSSpawner.cpp
//  ProjectSteve
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSSpawner.h"
#include "PSWorld.h"

#include "PSPlayer.h"
#include "PSAnimatable.h"
#include "PSSyringe.h"
#include "PSHelix.h"
#include "PSStevelet.h"

namespace PS
{
	RNDefineMeta(Spawner, RN::SceneNode)
	
	Spawner::Spawner(ObjectType type, size_t poolSize) : _type(type), _objectPool(new RN::Array()), _activeObjects(new RN::Array()), _currentObject(nullptr)
	{
		if(type == ObjectType::SyringeType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Syringe *syringe = new Syringe();
				syringe->SetSpawner(this);
				_objectPool->AddObject(syringe->Autorelease());
			}
		}
		else if(type == ObjectType::GeneCType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Gene *gene = new Gene(Gene::Type::C);
				gene->SetSpawner(this);
				_objectPool->AddObject(gene->Autorelease());
			}
		}
		else if(type == ObjectType::GeneGType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Gene *gene = new Gene(Gene::Type::G);
				gene->SetSpawner(this);
				_objectPool->AddObject(gene->Autorelease());
			}
		}
		else if(type == ObjectType::GeneAType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Gene *gene = new Gene(Gene::Type::A);
				gene->SetSpawner(this);
				_objectPool->AddObject(gene->Autorelease());
			}
		}
		else if(type == ObjectType::GeneTType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Gene *gene = new Gene(Gene::Type::T);
				gene->SetSpawner(this);
				_objectPool->AddObject(gene->Autorelease());
			}
		}
		else if(type == ObjectType::SteveletType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Stevelet *stevlet = new Stevelet();
				stevlet->SetSpawner(this);
				_objectPool->AddObject(stevlet->Autorelease());
			}
		}
	}

	void Spawner::Update(float delta)
	{
		SceneNode::Update(delta);
		
		if(_currentObject && _currentObject->GetWorldPosition().GetDistance(GetWorldPosition()) > 0.3f)
		{
			if(_type == ObjectType::SyringeType)
			{
				_currentObject->Downcast<Syringe>()->SetDNA(World::GetSharedInstance()->GetHelix()->GetDNA());
			}
			_currentObject = nullptr;
		}
		
		if(!_currentObject)
		{
			if(_objectPool->GetCount() > 0)
			{
				Grabbable *lastObject = _objectPool->GetLastObject<Grabbable>();
				_activeObjects->AddObject(lastObject);
				_objectPool->RemoveObject(lastObject);
				World::GetSharedInstance()->AddLevelNode(lastObject, true);
				lastObject->SetWorldPosition(GetWorldPosition());
				lastObject->SetWorldRotation(GetWorldRotation());
				_currentObject = lastObject;
				
				if(_currentObject)
				{
					if(_type == ObjectType::GeneCType || _type == ObjectType::GeneGType || _type == ObjectType::GeneAType || _type == ObjectType::GeneTType)
					{
						_currentObject->Downcast<Gene>()->EnablePhysics();
					}
					
					if(_type == ObjectType::SteveletType)
					{
						SteveStats stats;
						_currentObject->Downcast<Stevelet>()->SetSteveletStats(stats, false, nullptr);
					}
				}
			}
			else
			{
				if(_type == ObjectType::SteveletType) return;
				if(_activeObjects->GetCount() == 0) return;
				
				Grabbable *firstObject = _activeObjects->GetFirstObject<Grabbable>()->Retain();
				_activeObjects->RemoveObject(firstObject);
				_activeObjects->AddObject(firstObject->Autorelease());
				World::GetSharedInstance()->GetPlayer()->ReleaseObjectFromHandIfNeeded(firstObject);
				firstObject->Reset();
				firstObject->SetWorldPosition(GetWorldPosition());
				firstObject->SetWorldRotation(GetWorldRotation());
				_currentObject = firstObject;
			}
			
			if(_type == ObjectType::SyringeType)
			{
				_currentObject->Downcast<Syringe>()->SetDNA(World::GetSharedInstance()->GetHelix()->GetDNA());
			}
		}
	}

	void Spawner::ReturnToPool(RN::SceneNode *node)
	{
		if(!_activeObjects->ContainsObject(node)) return;
		
		_activeObjects->RemoveObject(node);
		_objectPool->AddObject(node);
		
		World::GetSharedInstance()->RemoveLevelNode(node);
	}
}
