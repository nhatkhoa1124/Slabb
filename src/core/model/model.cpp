// Must load core_vertex first
#include "core/core_interface/core_vertex.hpp"
#include "core/model/model.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tiny_gltf.h>
#include <spdlog/spdlog.h>


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

		Model out_model;
		for (const auto& mesh : model.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				Mesh out_mesh;

				const auto& pos_it = primitive.attributes.find("POSITION");
				if (pos_it == primitive.attributes.end()) continue;

				const tinygltf::Accessor& pos_accessor = model.accessors[pos_it->second];
				const tinygltf::BufferView& pos_view = model.bufferViews[pos_accessor.bufferView];
				const tinygltf::Buffer& pos_buffer = model.buffers[pos_view.buffer];
				
				const float* pos_data = reinterpret_cast<const float*>(
					&pos_buffer.data[pos_view.byteOffset + pos_accessor.byteOffset]);

				size_t vertex_count = pos_accessor.count;
				out_mesh.vertices.resize(vertex_count);
				// Read data out based on float offsets
				for (size_t i = 0; i < vertex_count; ++i)
				{
					out_mesh.vertices[i].position = DirectX::XMFLOAT3(
						pos_data[i * 3 + 0],
						pos_data[i * 3 + 1],
						pos_data[i * 3 + 2]
					);
					// Default initialize other attributes for testing
					out_mesh.vertices[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					out_mesh.vertices[i].normal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
					out_mesh.vertices[i].tex_coord = DirectX::XMFLOAT2(0.0f, 0.0f);
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
		spdlog::info("Successfully compiled model file: {} with {} submeshes", file_path, out_model.meshes.size());
		return out_model;
	}
}