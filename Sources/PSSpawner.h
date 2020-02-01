//
//  PSSpawner.h
//  ProjectSteve
//
//  Copyright 2020 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __ProjectSteve_SPAWNER_H_
#define __ProjectSteve_SPAWNER_H_

#include <Rayne.h>
#include <RNPhysXWorld.h>
#include <RNOpenALWorld.h>

namespace PS
{
	class Animatable;
	class Spawner : public RN::SceneNode
	{
	public:
		enum ObjectType
		{
			SteveletType,
			SyringeType,
			DNAType
		};
		
		Spawner(RN::String *file, ObjectType type, size_t poolSize);
		~Spawner() = default;
		void Update(float delta) override;
		
	private:
		ObjectType _type;
		RN::Array *_objectPool;
		RN::Array *_activeObjects;
		
		Animatable *_currentObject;
		
		RNDeclareMeta(Spawner)
	};
}

#endif /* defined(__ProjectSteve_SPAWNER_H_) */
