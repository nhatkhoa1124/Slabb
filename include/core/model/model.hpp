#pragma once
#include "common/common.hpp"
#include <vector>
#include <cstdint>
#include <string>

class Vertex;

namespace slabb::core::model
{
	struct Image
	{
		int width{ 0 };
		int height{ 0 };
		std::vector<uint8_t> pixels; // tightly packed RGBA8, row-major, no padding
	};

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		int base_color_texture{ -1 }; // index into Model::images, or -1 if untextured
	};

	struct Model
	{
		std::vector<Mesh> meshes;
		std::vector<Image> images;
	};

	class SLABB_EXPORT ModelLoader
	{
	public:
		[[nodiscard]] static Model load_gltf(const std::string& file_path);
	};
}