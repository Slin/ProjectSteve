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
	}

	void Helix::Update(float deltaTime)
	{
		Rotate(RN::Vector3(deltaTime * 16.0f, 0.0f, 0.0f));
	}
}
