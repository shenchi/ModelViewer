#pragma once

#include "TofuMath.h"

namespace tofu
{
	using math::int4;
	using math::float3;
	using math::float4;

	struct Bone
	{
		float		matrix[12];
		int32_t		parent;
		int32_t		firstChild;
		int32_t		nextSibling;
		int32_t		name;
	};

	struct Vertex
	{
		float3		position;
		float3		normal;
		float3		tangent;
		float3		uv;
		int4		bones;
		float4		weights;
	};

}