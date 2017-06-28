#pragma once

#include "TofuMath.h"

namespace tofu
{
	using math::int4;
	using math::float3;
	using math::float4;

	struct Vertex
	{
		float3		position;
		float3		normal;
		float3		tangent;
		float3		uv;
	};

	struct SkinnedVertex
	{
		float3		position;
		float3		normal;
		float3		tangent;
		float3		uv;
		int4		bones;
		float4		weights;
	};

	struct Bone
	{
		float		matrix[12];
		int32_t		parent;
		int32_t		firstChild;
		int32_t		nextSibling;
		int32_t		name;
	};

	struct Mesh
	{
		float		matrix[12];
		uint32_t	startVertex;
		uint32_t	startIndex;
		uint32_t	numVertices;
		uint32_t	numIndices;
	};

	struct Track
	{

	};

	struct Animation
	{
		uint32_t	frameRate;
		uint32_t	numFrames;
		uint32_t	numTracks;
	};

	struct TFModel
	{
		uint32_t	magic;
		uint32_t	version;
		uint32_t	flags;
		uint32_t	vertexStart;
		uint32_t	meshStart;
		uint32_t	boneStart;
		uint32_t	animStart;
		uint32_t	stringStart;
	};
}