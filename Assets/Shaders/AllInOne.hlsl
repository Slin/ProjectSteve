//
//  Shaders.hlsl
//  Rayne
//
//  Copyright 2015 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef RN_UV0
#define RN_UV0 0
#endif

#ifndef PS_SPRITESHEET
#define PS_SPRITESHEET 0
#endif

#if RN_UV0
[[vk::binding(3)]] SamplerState linearRepeatSampler : register(s0);
[[vk::binding(4)]] Texture2D texture0 : register(t0);
#endif

[[vk::binding(1)]] cbuffer vertexUniforms : register(b0)
{
	matrix modelViewProjectionMatrix;

#if PS_SPRITESHEET
	float4 specularColor;
#endif
};

[[vk::binding(2)]] cbuffer fragmentUniforms : register(b1)
{
	float4 cameraAmbientColor;
	float4 diffuseColor;
};

struct InputVertex
{
	[[vk::location(0)]] float3 position : POSITION;

#if RN_UV0
	[[vk::location(5)]] float2 texCoords0 : TEXCOORD0;
#endif
};

struct FragmentVertex
{
	float4 position : SV_POSITION;

#if RN_UV0
	float2 texCoords0 : TEXCOORD0;
#endif
};

FragmentVertex main_vertex(InputVertex vert)
{
	FragmentVertex result;

#if RN_UV0
	result.texCoords0 = vert.texCoords0;
	#if PS_SPRITESHEET
		result.texCoords0 *= specularColor.xy;
		result.texCoords0 += specularColor.zw;
	#endif
#endif

	result.position = mul(modelViewProjectionMatrix, float4(vert.position, 1.0));

	return result;
}


float4 main_fragment(FragmentVertex vert) : SV_TARGET
{
	float4 color = diffuseColor;

#if RN_UV0
	color *= texture0.Sample(linearRepeatSampler, vert.texCoords0);
	if(color.a > 0.5) color.a = 1.0;
#endif

	color.rgb *= cameraAmbientColor.rgb * color.a;
	return color;
}
