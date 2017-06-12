#pragma once

#include <cstdint>

struct Bone
{
	float		matrix[12];
	int32_t		parent;
	int32_t		firstChild;
	int32_t		nextSibling;
	int32_t		name;
};
