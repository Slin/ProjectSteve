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
	
	Helix::Helix()
	{
		//RN::ShaderLibrary *shaderLibrary = World::GetSharedInstance()->GetShaderLibrary();
		
		RN::Model *model = RN::Model::WithName(RNCSTR("models/dna.sgm"))->Copy();
		SetModel(model);
		
		RN::Model *blubb = RN::Model::WithName(RNCSTR("models/dna_blubb.sgm"));
		float currentHeight = 0.04f;
		for(int i = 0; i < 12; i++)
		{
			RN::Entity *blubbEntity = new RN::Entity(blubb);
			AddChild(blubbEntity);
			blubbEntity->SetPosition(RN::Vector3(0.0f, currentHeight, 0.0f));
			blubbEntity->SetRotation(RN::Vector3(200.0f/(0.08*12.0f) * currentHeight, 0.0f, 0.0f));
			
			currentHeight += 0.08f;
		}
	}

	void Helix::Update(float deltaTime)
	{
		Rotate(RN::Vector3(deltaTime * 16.0f, 0.0f, 0.0f));
	}
}
