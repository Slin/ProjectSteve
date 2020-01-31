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

#ifndef RN_UV1
#define RN_UV1 0
#endif

#ifndef PZ_UNLIT
#define PZ_UNLIT 0
#endif

#ifndef PZ_SKYLIGHT
#define PZ_SKYLIGHT 0
#endif

#ifndef PZ_SIMPLELIGHT
#define PZ_SIMPLELIGHT 0
#endif

#ifndef PZ_SPECULARITY
#define PZ_SPECULARITY 0
#endif

#ifndef PZ_FLOOR
#define PZ_FLOOR 0
#endif

#ifndef PZ_PLAYERFADE
#define PZ_PLAYERFADE 0
#endif

#ifndef PZ_FIX_LIGHTMAP_GAMMA
#define PZ_FIX_LIGHTMAP_GAMMA 0
#endif

#ifndef RN_ANIMATIONS
#define RN_ANIMATIONS 0
#endif

#ifndef RN_MAX_BONES
#define RN_MAX_BONES 100
#endif

#if RN_UV0 || RN_UV1 || PZ_SIMPLELIGHT || PZ_SKYLIGHT || PZ_FLOOR
[[vk::binding(3)]] SamplerState linearRepeatSampler : register(s0);
#endif

#if RN_UV0
[[vk::binding(4)]] Texture2D texture0 : register(t0);

	#if RN_UV1 || PZ_SIMPLELIGHT || PZ_SKYLIGHT
		[[vk::binding(5)]] Texture2D texture1 : register(t1);
	#endif
#elif RN_UV1 || PZ_SIMPLELIGHT || PZ_SKYLIGHT
[[vk::binding(4)]] Texture2D texture1 : register(t0);
#endif

#if PZ_FLOOR
[[vk::binding(6)]] TextureCube texture2: register(t2);
#endif

[[vk::binding(1)]] cbuffer vertexUniforms : register(b0)
{
	matrix modelMatrix;
	matrix modelViewProjectionMatrix;

#if RN_ANIMATIONS
	matrix boneMatrices[RN_MAX_BONES];
#endif

#if PZ_PLAYERFADE
	float4 specularColor;
#endif
};

struct PointLight
{
	float4 positionAndRange;
	float4 color;
};

[[vk::binding(2)]] cbuffer fragmentUniforms : register(b1)
{
	float4 cameraAmbientColor;
	float4 diffuseColor;

#if !RN_UV1 || PZ_UNLIT
	float4 ambientColor;
#endif

	PointLight pointLights[8];

#if PZ_FLOOR || PZ_SPECULARITY
	float3 cameraPosition;
#endif
};

struct InputVertex
{
	[[vk::location(0)]] float3 position : POSITION;
	[[vk::location(1)]] float3 normal : NORMAL;

#if RN_UV0
	[[vk::location(5)]] float2 texCoords0 : TEXCOORD0;
#endif
#if RN_UV1
	[[vk::location(6)]] float2 texCoords1 : TEXCOORD1;
#endif

#if RN_ANIMATIONS
	[[vk::location(7)]] float4 boneWeights : BONEWEIGHTS;
	[[vk::location(8)]] float4 boneIndices : BONEINDICES;
#endif
};

struct FragmentVertex
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;

#if PZ_SIMPLELIGHT || PZ_SKYLIGHT || PZ_FLOOR || PZ_SPECULARITY
	float3 worldNormal : NORMAL;
#endif

#if RN_UV0
	float2 texCoords0 : TEXCOORD0;
#endif
#if RN_UV1
	float2 texCoords1 : TEXCOORD1;
#endif

#if PZ_PLAYERFADE
	float playerFadeAlpha : TEXCOORD2;
#endif
};

#if RN_ANIMATIONS
float4 getAnimatedPosition(float4 position, float4 weights, float4 indices)
{
	float4 pos1 = mul(boneMatrices[int(indices.x)], position);
	float4 pos2 = mul(boneMatrices[int(indices.y)], position);
	float4 pos3 = mul(boneMatrices[int(indices.z)], position);
	float4 pos4 = mul(boneMatrices[int(indices.w)], position);

	float4 pos = pos1 * weights.x + pos2 * weights.y + pos3 * weights.z + pos4 * weights.w;
	pos.w = position.w;

	return pos;
}
#endif

FragmentVertex main_vertex(InputVertex vert)
{
	FragmentVertex result;

#if RN_UV0
	result.texCoords0 = vert.texCoords0;
#endif
#if RN_UV1
	result.texCoords1 = vert.texCoords1;
#endif

#if PZ_PLAYERFADE
	result.playerFadeAlpha = saturate((vert.position.y - specularColor.r) * specularColor.g);
#endif

#if RN_ANIMATIONS
	float4 position = getAnimatedPosition(float4(vert.position, 1.0), vert.boneWeights, vert.boneIndices);
#else
	float4 position = float4(vert.position, 1.0);
#endif

	result.position = mul(modelViewProjectionMatrix, position);
	result.worldPosition = mul(modelMatrix, position).xyz;

	#if PZ_SIMPLELIGHT || PZ_SKYLIGHT || PZ_FLOOR || PZ_SPECULARITY
		#if RN_ANIMATIONS
			float4 normal = getAnimatedPosition(float4(vert.normal, 0.0), vert.boneWeights, vert.boneIndices);
		#else
			float4 normal = float4(vert.normal, 0.0);
		#endif

		result.worldNormal = mul(modelMatrix, normal).xyz;
	#endif

	return result;
}


float4 main_fragment(FragmentVertex vert) : SV_TARGET
{
#if PZ_UNLIT
	float4 light = ambientColor;
#else

	#if RN_UV1
		float4 light = float4(1.0, 1.0, 1.0, 1.0);
		float value = texture1.SampleLevel(linearRepeatSampler, vert.texCoords1, 0).r;
		#if PZ_FIX_LIGHTMAP_GAMMA
			light.rgb = value * value;
		#else
			light.rgb = value;
		#endif
	#else
		float4 light = ambientColor;
	#endif

	#if PZ_SKYLIGHT
		float lights = saturate(normalize(vert.worldNormal).y) + saturate(-normalize(vert.worldNormal).y)*0.3;
		float brightness = texture1.Sample(linearRepeatSampler, (-vert.worldPosition.xz + float2(38.0, -12.0))/80.0).r;
		light.rgb = light.rgb * (brightness * 0.6 + 0.3) + lights * brightness * 2.0;
	#elif PZ_SIMPLELIGHT
		light.rgb = texture1.Sample(linearRepeatSampler, (-vert.worldPosition.xz + float2(38.0, -12.0))/80.0).r * 2.0;
	#elif PZ_FLOOR
		float lights = 0;
		for(int i = 0; i < 8; i++)
		{
			float3 lightDirection = pointLights[i].positionAndRange.xyz - vert.worldPosition.xyz;
			float3 lightDirectionNorm = normalize(lightDirection);
			float lightDistance = sqrt(dot(lightDirection, lightDirection));
			float lightFactor = saturate(1.0 - lightDistance / pointLights[i].positionAndRange.w);
			lights = max(lightFactor, lights);
		}
		light.rgb *= 1.0-lights;
	#endif
#endif

	float4 color = diffuseColor * light;

#if RN_UV0
	color.a = diffuseColor.a;
	color *= texture0.Sample(linearRepeatSampler, vert.texCoords0);
#endif

#if PZ_FLOOR
	float3 incidentVector = normalize(vert.worldPosition - cameraPosition);
	float3 worldNormal = normalize(vert.worldNormal);
	float3 reflectionDir = reflect(incidentVector, worldNormal);
	float3 reflections = texture2.Sample(linearRepeatSampler, reflectionDir).rgb;

	float reflectionFactor = 0.3 * pow(1.0 + dot(incidentVector, worldNormal), 5);
	color.rgb = lerp(color.rgb, reflections, reflectionFactor);
#endif

#if PZ_SPECULARITY
	float3 incidentVector2 = normalize(cameraPosition - vert.worldPosition);
	float3 worldNormal2 = normalize(vert.worldNormal);
	float3 halfVector = (incidentVector2 + float3(0.0, 1.0, 0.0)) * 0.5;
	float specularity = saturate(dot(halfVector, worldNormal2));
	float backlight = 1.0 - saturate(dot(incidentVector2, worldNormal2));
	color.rgb += pow(specularity, 12.0)*1.5 + backlight * 0.2;
#endif

#if PZ_PLAYERFADE
	color.a *= vert.playerFadeAlpha;
#endif

	color.rgb *= cameraAmbientColor.rgb * color.a;
	return color;
}
