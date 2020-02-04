//
//  PSPlayer.cpp
//  ProjectSteve
//
//  Copyright 2018 by SlinDev & BlueIceshard. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "PSAnimatable.h"
#include "PSWorld.h"

namespace PS
{
	RNDefineMeta(Animatable, Grabbable)
	
	Animatable::Animatable(RN::String const* spriteName) 
		: _animationTimer(RN::RandomNumberGenerator::GetSharedGenerator()->GetRandomFloatRange(0.0f, 1.0f))
	{
		bool isSprite = false;
		if (!spriteName) _model = RN::Model::WithName(RNCSTR("sprites/stevelet.sgm"))->Copy();
		else {
			if (spriteName->HasSuffix(RNCSTR(".sgm")))
			{
				_model = RN::Model::WithName(spriteName)->Copy();
			}
			else
			{
				_model = RN::Model::WithName(RNCSTR("sprites/stevelet.sgm"))->Copy();
				isSprite = true;
			}
		}
		
		RN::ShaderLibrary *shaderLibrary = World::GetSharedInstance()->GetShaderLibrary();
		
		RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(_model->GetLODStage(0)->GetMeshAtIndex(0));
		shaderOptions->AddDefine(RNCSTR("PS_SPRITESHEET"), RNCSTR("1"));
		
		RN::Material *material = _model->GetLODStage(0)->GetMaterialAtIndex(0)->Copy();
		_model->GetLODStage(0)->ReplaceMaterial(material, 0);
		if(isSprite)
		{
			material->RemoveAllTextures();
			material->AddTexture(RN::Texture::WithName(spriteName));
		} else if(!spriteName) material->RemoveAllTextures();
		material->SetAlphaToCoverage(true);
		material->SetVertexShader(shaderLibrary->GetShaderWithName(RNCSTR("main_vertex"), shaderOptions), RN::Shader::UsageHint::Default);
		material->SetFragmentShader(shaderLibrary->GetShaderWithName(RNCSTR("main_fragment"), shaderOptions), RN::Shader::UsageHint::Default);
		material->SetSpecularColor(RN::Color::WithRGBA(1.0f, 1.0f, 0.0f, 0.0f));
		SetModel(_model);
	}

	void Animatable::Update(float delta)
	{
		Grabbable::Update(delta);
		
		if(!isAnimated) return;
		
		_animationTimer += delta*2.0f;
		
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
