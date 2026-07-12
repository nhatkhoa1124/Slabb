// Must load core_vertex first
#include "core/core_interface/core_vertex.hpp"
#include "core/model/model.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>
#include <spdlog/spdlog.h>
#include <cstring>


namespace slabb::core::model
{
	using slabb::core::Vertex;

	Model ModelLoader::load_gltf(const std::string& file_path)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool success = false;
		if (file_path.substr(file_path.find_last_of(".") + 1) == "glb")
		{
			success = loader.LoadBinaryFromFile(&model, &err, &warn, file_path);
		}
		else
		{
			success = loader.LoadASCIIFromFile(&model, &err, &warn, file_path);
		}

		if (!warn.empty())  spdlog::warn("glTF Warning: {}", warn);
		if (!err.empty())   spdlog::error("glTF Error: {}", err);
		if (!success)       throw std::runtime_error("Failed to parse glTF file: " + file_path);

		std::vector<Image> pending_images; // populated during mesh iteration, attached after
		std::vector<int> image_map(model.images.size(), -1);
		auto import_image = [&](int gltf_image_index) -> int
		{
			if (gltf_image_index < 0 || gltf_image_index >= static_cast<int>(model.images.size()))
				return -1;
			if (image_map[gltf_image_index] != -1)
				return image_map[gltf_image_index];

			const auto& src = model.images[gltf_image_index];
			Image img;
			img.width = src.width;
			img.height = src.height;

			const int pixel_count = img.width * img.height;
			const int src_channels = src.component;
			img.pixels.resize(static_cast<size_t>(pixel_count) * 4u);

			// tinygltf stores RGBA when component==4, RGB when 3. Anything else (grey, GA) is
			// rare for PBR base color; expand to opaque grey so the texture manager still sees
			// valid RGBA8 data.
			if (src_channels == 4)
			{
				std::memcpy(img.pixels.data(), src.image.data(), img.pixels.size());
			}
			else if (src_channels == 3)
			{
				for (int i = 0; i < pixel_count; ++i)
				{
					img.pixels[i * 4 + 0] = src.image[i * 3 + 0];
					img.pixels[i * 4 + 1] = src.image[i * 3 + 1];
					img.pixels[i * 4 + 2] = src.image[i * 3 + 2];
					img.pixels[i * 4 + 3] = 255;
				}
			}
			else if (src_channels == 1)
			{
				for (int i = 0; i < pixel_count; ++i)
				{
					const uint8_t g = src.image[i];
					img.pixels[i * 4 + 0] = g;
					img.pixels[i * 4 + 1] = g;
					img.pixels[i * 4 + 2] = g;
					img.pixels[i * 4 + 3] = 255;
				}
			}
			else
			{
				std::memset(img.pixels.data(), 128, img.pixels.size());
				for (int i = 0; i < pixel_count; ++i) img.pixels[i * 4 + 3] = 255;
			}

			const int local_index = static_cast<int>(pending_images.size());
			pending_images.push_back(std::move(img));
			image_map[gltf_image_index] = local_index;
			return local_index;
		};

		Model out_model;
		for (const auto& mesh : model.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				Mesh out_mesh;

				// Resolve base-color texture if this primitive references a glTF material.
				if (primitive.material >= 0 && primitive.material < static_cast<int>(model.materials.size()))
				{
					const auto& mat = model.materials[primitive.material];
					const int tex_idx = mat.pbrMetallicRoughness.baseColorTexture.index;
					if (tex_idx >= 0 && tex_idx < static_cast<int>(model.textures.size()))
					{
						const int src_image = model.textures[tex_idx].source;
						out_mesh.base_color_texture = import_image(src_image);
					}
				}

				// Extract position data
				const auto& pos_it = primitive.attributes.find("POSITION");
				if (pos_it == primitive.attributes.end()) continue;
				const tinygltf::Accessor& pos_accessor = model.accessors[pos_it->second];
				const tinygltf::BufferView& pos_view = model.bufferViews[pos_accessor.bufferView];
				const tinygltf::Buffer& pos_buffer = model.buffers[pos_view.buffer];
				
				const float* pos_data = reinterpret_cast<const float*>(
					&pos_buffer.data[pos_view.byteOffset + pos_accessor.byteOffset]);
				size_t vertex_count = pos_accessor.count;
				out_mesh.vertices.resize(vertex_count);

				// Extract normal data
				const float* normal_data = nullptr;
				const auto& norm_it = primitive.attributes.find("NORMAL");
				if (norm_it != primitive.attributes.end())
				{
					const tinygltf::Accessor& accessor = model.accessors[norm_it->second];
					const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
					normal_data = reinterpret_cast<const float*>(&model.buffers[view.buffer].data[view.byteOffset + accessor.byteOffset]);
				}

				// Extract texture data
				const float* tex_data = nullptr;
				const auto& tex_it = primitive.attributes.find("TEXCOORD_0");
				if (tex_it != primitive.attributes.end())
				{
					const tinygltf::Accessor& accessor = model.accessors[tex_it->second];
					const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
					tex_data = reinterpret_cast<const float*>(&model.buffers[view.buffer].data[view.byteOffset + accessor.byteOffset]);
				}

				// Extract color data
				const float* color_data = nullptr;
				int color_type = TINYGLTF_TYPE_VEC4; // glTF colors can be VEC3 or VEC4
				const auto& col_it = primitive.attributes.find("COLOR_0");
				if (col_it != primitive.attributes.end())
				{
					const tinygltf::Accessor& accessor = model.accessors[col_it->second];
					const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
					color_data = reinterpret_cast<const float*>(&model.buffers[view.buffer].data[view.byteOffset + accessor.byteOffset]);
					color_type = accessor.type;
				}

				for (size_t i = 0; i < vertex_count; ++i)
				{
					slabb::core::Vertex& v = out_mesh.vertices[i];

					// Positions (Guaranteed)
					v.position = DirectX::XMFLOAT3(pos_data[i * 3 + 0], pos_data[i * 3 + 1], pos_data[i * 3 + 2]);

					// Normals (Fallback to upward vector if missing)
					if (normal_data) {
						v.normal = DirectX::XMFLOAT3(normal_data[i * 3 + 0], normal_data[i * 3 + 1], normal_data[i * 3 + 2]);
					}
					else {
						v.normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
					}

					// UV Coordinates (Fallback to zero if missing)
					if (tex_data) {
						v.tex_coord = DirectX::XMFLOAT2(tex_data[i * 2 + 0], tex_data[i * 2 + 1]);
					}
					else {
						v.tex_coord = DirectX::XMFLOAT2(0.0f, 0.0f);
					}

					// Vertex Colors (Fallback to opaque white if missing)
					if (color_data) {
						if (color_type == TINYGLTF_TYPE_VEC4) {
							v.color = DirectX::XMFLOAT4(color_data[i * 4 + 0], color_data[i * 4 + 1], color_data[i * 4 + 2], color_data[i * 4 + 3]);
						}
						else if (color_type == TINYGLTF_TYPE_VEC3) {
							v.color = DirectX::XMFLOAT4(color_data[i * 3 + 0], color_data[i * 3 + 1], color_data[i * 3 + 2], 1.0f);
						}
					}
					else {
						v.color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					}
				}
				if (primitive.indices >= 0)
				{
					const tinygltf::Accessor& index_accessor = model.accessors[primitive.indices];
					const tinygltf::BufferView& index_view = model.bufferViews[index_accessor.bufferView];
					const tinygltf::Buffer& index_buffer = model.buffers[index_view.buffer];

					const void* raw_index_data = &index_buffer.data[index_view.byteOffset + index_accessor.byteOffset];
					out_mesh.indices.reserve(index_accessor.count);

					// glTF indices can be stored as 8-bit, 16-bit, or 32-bit unsigned integers!
					if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
					{
						const uint32_t* data = static_cast<const uint32_t*>(raw_index_data);
						for (size_t i = 0; i < index_accessor.count; ++i) out_mesh.indices.push_back(data[i]);
					}
					else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
					{
						const uint16_t* data = static_cast<const uint16_t*>(raw_index_data);
						for (size_t i = 0; i < index_accessor.count; ++i) out_mesh.indices.push_back(data[i]);
					}
					else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
					{
						const uint8_t* data = static_cast<const uint8_t*>(raw_index_data);
						for (size_t i = 0; i < index_accessor.count; ++i) out_mesh.indices.push_back(data[i]);
					}
				}

				out_model.meshes.push_back(std::move(out_mesh));
			}
		}
		out_model.images = std::move(pending_images);
		spdlog::info("Successfully compiled model file: {} with {} submeshes and {} unique images",
			file_path, out_model.meshes.size(), out_model.images.size());
		return out_model;
	}
}