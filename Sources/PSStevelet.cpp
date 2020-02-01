//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSStevelet.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Stevelet, RN::Entity)
	
	Stevelet::Stevelet() : _animationTimer(0.0f)
	{
		RN::ShaderLibrary *shaderLibrary = World::GetSharedInstance()->GetShaderLibrary();
		
		RN::Model *model = RN::Model::WithName(RNCSTR("models/stevelet.sgm"))->Copy();
		
		RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(model->GetLODStage(0)->GetMeshAtIndex(0));
		shaderOptions->AddDefine(RNCSTR("PS_SPRITESHEET"), RNCSTR("1"));
		
		RN::Material *material = model->GetLODStage(0)->GetMaterialAtIndex(0);
		material->SetAlphaToCoverage(true);
		material->SetVertexShader(shaderLibrary->GetShaderWithName(RNCSTR("main_vertex"), shaderOptions), RN::Shader::UsageHint::Default);
		material->SetFragmentShader(shaderLibrary->GetShaderWithName(RNCSTR("main_fragment"), shaderOptions), RN::Shader::UsageHint::Default);
		material->SetSpecularColor(RN::Color::WithRGBA(1.0f, 1.0f, 0.0f, 0.0f));
		SetModel(model);
	}

	void Stevelet::Update(float deltaTime)
	{
		_animationTimer += deltaTime*2.0f;
		
		while(_animationTimer > 1.0f)
		{
			_animationTimer -= 1.0f;
		}
		
		float animationOffset = 0.0f;
		if(_animationTimer > 0.25f) animationOffset = 0.25f;
		if(_animationTimer > 0.5f) animationOffset = 0.5f;
		if(_animationTimer > 0.75f) animationOffset = 0.75f;
		
		RN::Material *material = GetModel()->GetLODStage(0)->GetMaterialAtIndex(0);
		material->SetSpecularColor(RN::Color::WithRGBA(1.0f, 1.0f, 0.0f, animationOffset));
	}
}
