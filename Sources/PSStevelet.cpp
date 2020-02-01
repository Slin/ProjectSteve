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
	
	Stevelet::Stevelet() {
		RN::Mesh* cube = RN::Mesh::WithTexturedCube(
				{1, 1, 1});
		RN::Material* mat = new RN::Material(
			RN::Renderer::GetActiveRenderer()->GetDefaultShader(
				RN::Shader::Type::Vertex,
				RN::Shader::Options::WithMesh(cube),
				RN::Shader::UsageHint::Default),
			RN::Renderer::GetActiveRenderer()->GetDefaultShader(
				RN::Shader::Type::Fragment,
				RN::Shader::Options::WithMesh(cube),
				RN::Shader::UsageHint::Default));
		mat->AddTexture(RN::Texture::WithName(RNCSTR("sprites/Pyxel2DBlob_Arms_Idle.png")));

		SetModel(new RN::Model(cube, mat));
	}

	void Stevelet::Update(float deltaTime) {
		
	}
}
