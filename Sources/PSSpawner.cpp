//
//  PSSpawner.cpp
//  ProjectSteve
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSSpawner.h"
#include "PSWorld.h"

#include "PSAnimatable.h"
#include "PSSyringe.h"

namespace PS
{
	RNDefineMeta(Spawner, RN::SceneNode)
	
	Spawner::Spawner(RN::String *file, ObjectType type, size_t poolSize) : _type(type), _objectPool(new RN::Array()), _activeObjects(new RN::Array()), _currentObject(nullptr)
	{
		if(type == ObjectType::SyringeType)
		{
			for(int i = 0; i < poolSize; i++)
			{
				Syringe *syringe = new Syringe();
				_objectPool->AddObject(syringe->Autorelease());
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
				Animatable *lastObject = _objectPool->GetLastObject<Animatable>();
				_activeObjects->AddObject(lastObject);
				_objectPool->RemoveObject(lastObject);
				World::GetSharedInstance()->AddLevelNode(lastObject, true);
				lastObject->SetWorldPosition(GetWorldPosition());
				lastObject->SetWorldRotation(GetWorldRotation());
				_currentObject = lastObject;
			}
			else
			{
				Animatable *firstObject = _activeObjects->GetFirstObject<Animatable>();
				_activeObjects->RemoveObject(firstObject);
				_activeObjects->AddObject(firstObject);
				firstObject->Reset();
				firstObject->SetWorldPosition(GetWorldPosition());
				firstObject->SetWorldRotation(GetWorldRotation());
				_currentObject = firstObject;
			}
		}
	}
}
