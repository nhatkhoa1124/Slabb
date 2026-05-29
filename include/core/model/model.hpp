#pragma once
#include "common/common.hpp"
#include <vector>
#include <cstdint>
#include <DirectXMath.h>

class Vertex;

namespace slabb::core::model
{
	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
	
	struct Material
	{

	};

	struct Model
	{
		std::vector<Mesh> meshes;
		DirectX::XMMATRIX transform;
	};
}