#include "graphics/scene.hpp"
#include <spdlog/spdlog.h>
#include <unordered_map>

#include "graphics/texture_manager.hpp"
#include "graphics/wrapper/resource/descriptor_heap.hpp"

namespace slabb::graphics
{
	void Scene::bind_runtime(TextureManager& texture_manager,
							 wrapper::descriptor::DescriptorHeap& resource_heap)
	{
		m_texture_manager = &texture_manager;
		m_resource_heap = &resource_heap;
		m_next_srv_slot = 0;
	}

	void Scene::load_model(const GraphicsModel& model)
	{
		assert(m_texture_manager && m_resource_heap && "Scene::bind_runtime must be called first");
		TextureManager& texture_manager = *m_texture_manager;
		wrapper::descriptor::DescriptorHeap& resource_heap = *m_resource_heap;

		spdlog::info("Extracting and loading model...");
		RenderModel render_model;
		render_model.transform = model.transform;

		std::string model_id = std::to_string(m_models.size());

		// Build an image -> (TextureResource*, srv_slot_index) map across calls to this function
		// so re-loading the same image from a different mesh (typical for glTF primitive sharing)
		// reuses one upload and one SRV slot. m_textures stores the unique textures; the per-mesh
		// RenderMesh just points into it.
		struct TexCacheEntry { TextureResource* tex; UINT srv_slot; };
		std::unordered_map<int, TexCacheEntry> texture_cache;
		for (size_t mesh_idx = 0; mesh_idx < model.meshes.size(); ++mesh_idx)
		{
			const auto& mesh = model.meshes[mesh_idx];
			RenderMesh gpu_mesh;
			gpu_mesh.vertex_count = static_cast<uint32_t>(mesh.vertex_count);
			gpu_mesh.index_count = static_cast<uint32_t>(mesh.index_count);

			std::string vb_name = "VertexBuffer_" + model_id + "_" + std::to_string(mesh_idx);
			gpu_mesh.vertex_buffer = texture_manager.graph().create_resource<BufferResource>(
				vb_name, BufferUsage::VERTEX);
			gpu_mesh.vertex_buffer->stage_data(mesh.vertex_data, mesh.vertex_count * mesh.vertex_stride);
			gpu_mesh.vertex_buffer->initialize_hardware(texture_manager.device(), static_cast<UINT>(mesh.vertex_stride));

			if (mesh.index_count != 0)
			{
				std::string ib_name = "IndexBuffer_" + model_id + "_" + std::to_string(mesh_idx);
				gpu_mesh.index_buffer = texture_manager.graph().create_resource<BufferResource>(
					ib_name, BufferUsage::INDEX);
				gpu_mesh.index_buffer->stage_data(mesh.index_data, mesh.index_count * sizeof(uint32_t));
				gpu_mesh.index_buffer->initialize_hardware(texture_manager.device(), sizeof(uint32_t), DXGI_FORMAT_R32_UINT);
			}

			if (mesh.base_color_texture >= 0)
			{
				auto it = texture_cache.find(mesh.base_color_texture);
				if (it == texture_cache.end())
				{
					const auto& img = model.images[mesh.base_color_texture];
					std::string tex_name = "Texture_" + model_id + "_" +
						std::to_string(mesh.base_color_texture);
					TextureResource* tex = texture_manager.create_rgba8(
						tex_name,
						static_cast<UINT>(img.width),
						static_cast<UINT>(img.height),
						img.pixels.data());

					// Allocate an SRV slot on the resource heap and bind the texture. The slot
					// index is the only stable identifier the shader-visible heap carries.
					const UINT slot = m_next_srv_slot++;
					// Returns a CPU+GPU handle pair; we only need the slot index here, which
					// the GPU side already encodes relative to the heap base.
					(void)resource_heap.create_shader_resource_view(
						texture_manager.device(), tex->underlying_resource(), slot);

					gpu_mesh.texture = tex;
					gpu_mesh.srv_slot = slot;

					m_textures.push_back(tex);
					texture_cache[mesh.base_color_texture] = { tex, slot };
				}
				else
				{
					// Reuse the cached texture (same upload + same SRV slot).
					gpu_mesh.texture = it->second.tex;
					gpu_mesh.srv_slot = it->second.srv_slot;
				}
			}

			render_model.sub_meshes.push_back(gpu_mesh);
		}

		m_models.push_back(render_model);
		spdlog::info("Model loaded successfully! ");
	}

	void Scene::update_transforms(const FrameContext& frame)
	{
		m_rotation_angle += 0.02f;

		TransformCB updated_payload{
			.mvp_matrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(m_rotation_angle))
		};

		frame.camera_constant_buffer->stage_data(&updated_payload, sizeof(TransformCB));
		frame.camera_constant_buffer->hardware_heap()->upload_data(frame.camera_constant_buffer->raw_data());
	}

	void Scene::collect_render_items()
	{
		m_render_queue.clear();
		assert(m_resource_heap && "Scene::bind_runtime must be called first");

		// Capture heap base + descriptor stride on the queue. The render pass uses these to
		// compute a per-item GPU descriptor handle from each mesh's SRV slot.
		const D3D12_GPU_DESCRIPTOR_HANDLE base = m_resource_heap->get_resource_heap_start();
		m_render_queue.srv_heap_base = base;
		m_render_queue.srv_descriptor_size = m_resource_heap->resource_heap_size();

		// helper: base + slot * size as GPU handle
		auto gpu_handle_for_slot = [&](UINT slot) -> D3D12_GPU_DESCRIPTOR_HANDLE
		{
			D3D12_GPU_DESCRIPTOR_HANDLE h = base;
			h.ptr += static_cast<SIZE_T>(slot) * static_cast<SIZE_T>(m_render_queue.srv_descriptor_size);
			return h;
		};

		for (const auto& model : m_models)
		{
			DirectX::XMMATRIX world_pos = model.transform;

			for (const auto& mesh : model.sub_meshes)
			{
				const bool has_texture = mesh.texture != nullptr;
				RenderItem item{
					.vertex_buffer = mesh.vertex_buffer,
					.index_buffer = mesh.index_buffer,
					.index_count = mesh.index_count,
					.vertex_count = mesh.vertex_count,
					.world_matrix = world_pos,
					.srv_gpu_handle = has_texture ? gpu_handle_for_slot(mesh.srv_slot)
												  : D3D12_GPU_DESCRIPTOR_HANDLE{}
				};
				m_render_queue.opaque_buckets.push_back(item);
			}
		}
	}
}