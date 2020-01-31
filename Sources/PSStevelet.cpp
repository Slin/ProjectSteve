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
		RN::Mesh* cube
			= RN::Mesh::WithColoredCube(
				{1,1,1},
				{.5f, .3f, .8f}
			);
		SetModel(
			new RN::Model(
				cube,
				new RN::Material(
					RN::Renderer::GetActiveRenderer()->GetDefaultShader(
						RN::Shader::Type::Vertex,
						RN::Shader::Options::WithMesh(cube),
						RN::Shader::UsageHint::Default
					),
					RN::Renderer::GetActiveRenderer()->GetDefaultShader(
						RN::Shader::Type::Fragment,
						RN::Shader::Options::WithMesh(cube),
						RN::Shader::UsageHint::Default
					)
				)
			));
	}

	void Stevelet::Update(float deltaTime) {
		
	}
}
