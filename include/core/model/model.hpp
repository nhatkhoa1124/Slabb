#pragma once
#include "common/common.hpp"
#include <vector>
#include <cstdint>
#include <string>

class Vertex;

namespace slabb::core::model
{
	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	struct Model
	{
		std::vector<Mesh> meshes;
	};

	class SLABB_EXPORT ModelLoader
	{
	public:
		[[nodiscard]] static Model load_gltf(const std::string& file_path);
	};
}