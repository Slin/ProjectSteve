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
				_objectPool->AddObject(syringe->Autorelease());
			}
		}
		else if(type == ObjectType::GeneType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Gene *gene = new Gene(RN::Model::WithName(RNCSTR("models/dna_blubb.sgm")));
				_objectPool->AddObject(gene->Autorelease());
			}
		}
		else if(type == ObjectType::SteveletType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Stevelet *stevlet = new Stevelet();
				_objectPool->AddObject(stevlet->Autorelease());
			}
		}
	}

	void Spawner::Update(float delta)
	{
		SceneNode::Update(delta);
		
		if(_currentObject && _currentObject->GetWorldPosition().GetDistance(GetWorldPosition()) > 0.3f)
		{
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
					if(_type == ObjectType::GeneType)
					{
						_currentObject->Downcast<Gene>()->EnablePhysics();
					}
				}
			}
			else
			{
				Grabbable *firstObject = _activeObjects->GetFirstObject<Grabbable>();
				_activeObjects->RemoveObject(firstObject);
				_activeObjects->AddObject(firstObject);
				World::GetSharedInstance()->GetPlayer()->ReleaseObjectFromHandIfNeeded(firstObject);
				firstObject->Reset();
				firstObject->SetWorldPosition(GetWorldPosition());
				firstObject->SetWorldRotation(GetWorldRotation());
				_currentObject = firstObject;
			}
		}
	}
}
