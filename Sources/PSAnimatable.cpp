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
	RNDefineMeta(Animatable, RN::Entity)
	
	Animatable::Animatable(RN::String const* spriteName) 
		: _animationTimer(0.0f), 
		_model(RN::Model::WithName(RNCSTR("models/stevelet.sgm"))->Copy()),
		_isGrabbed(false),
		_wantsThrow(false)
	{
		RN::ShaderLibrary *shaderLibrary = World::GetSharedInstance()->GetShaderLibrary();
		
		RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(_model->GetLODStage(0)->GetMeshAtIndex(0));
		shaderOptions->AddDefine(RNCSTR("PS_SPRITESHEET"), RNCSTR("1"));
		
		RN::Material *material = _model->GetLODStage(0)->GetMaterialAtIndex(0)->Copy();
		_model->GetLODStage(0)->ReplaceMaterial(material, 0);
		material->RemoveAllTextures();
		material->AddTexture(RN::Texture::WithName(spriteName));
		material->SetAlphaToCoverage(true);
		material->SetVertexShader(shaderLibrary->GetShaderWithName(RNCSTR("main_vertex"), shaderOptions), RN::Shader::UsageHint::Default);
		material->SetFragmentShader(shaderLibrary->GetShaderWithName(RNCSTR("main_fragment"), shaderOptions), RN::Shader::UsageHint::Default);
		material->SetSpecularColor(RN::Color::WithRGBA(1.0f, 1.0f, 0.0f, 0.0f));
		SetModel(_model);
	}

	void Animatable::Update(float delta)
	{
		RN::Entity::Update(delta);
		
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
		
		if(_isGrabbed)
		{
			RN::Vector3 currentSpeed = (GetWorldPosition() - _previousPosition) / delta;
			_currentGrabbedSpeed = _currentGrabbedSpeed.GetLerp(currentSpeed, 0.5f);
		}
		
		_previousPosition = GetWorldPosition();
	}

	void Animatable::SetIsGrabbed(bool isGrabbed)
	{
		if(_isGrabbed != isGrabbed)
		{
			if(isGrabbed)
			{
				_currentGrabbedSpeed = RN::Vector3();
			}
			_wantsThrow = !isGrabbed;
		}
		_isGrabbed = isGrabbed;
	}
}
