#include "graphics/scene.hpp"
#include <spdlog/spdlog.h>

namespace slabb::graphics
{
	void Scene::load_model(const GraphicsModel& model, RenderGraph& render_graph, ID3D12Device* device)
	{
		spdlog::info("Extracting and loading model...");
		RenderModel render_model;
		render_model.transform = model.transform;

		std::string model_id = std::to_string(m_models.size());

		for (const auto& mesh : model.meshes)
		{
			RenderMesh gpu_mesh;
			gpu_mesh.vertex_count = static_cast<uint32_t>(mesh.vertex_count);
			gpu_mesh.index_count = static_cast<uint32_t>(mesh.index_count);

			gpu_mesh.vertex_buffer = render_graph.create_resource<BufferResource>("VertexBuffer_" + model_id, BufferUsage::VERTEX);
			gpu_mesh.vertex_buffer->stage_data(mesh.vertex_data, mesh.vertex_count * mesh.vertex_stride);
			gpu_mesh.vertex_buffer->initialize_hardware(device, static_cast<UINT>(mesh.vertex_stride));

			if (mesh.index_count != 0)
			{
				gpu_mesh.index_buffer = render_graph.create_resource<BufferResource>("IndexBuffer_" + model_id, BufferUsage::INDEX);
				gpu_mesh.index_buffer->stage_data(mesh.index_data, mesh.index_count * sizeof(uint32_t));
				gpu_mesh.index_buffer->initialize_hardware(device, sizeof(uint32_t), DXGI_FORMAT_R32_UINT);
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
			.mvp_matrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationZ(m_rotation_angle))
		};

		frame.camera_constant_buffer->stage_data(&updated_payload, sizeof(TransformCB));
		frame.camera_constant_buffer->hardware_heap()->upload_data(frame.camera_constant_buffer->raw_data());
	}

	void Scene::collect_render_items()
	{
		m_render_queue.clear();

		for (const auto& model : m_models)
		{
			DirectX::XMMATRIX world_pos = model.transform;

			for (const auto& mesh : model.sub_meshes)
			{
				RenderItem item{
					.vertex_buffer = mesh.vertex_buffer,
					.index_buffer = mesh.index_buffer,
					.index_count = mesh.index_count,
					.vertex_count = mesh.vertex_count,
					.world_matrix = world_pos
				};
				m_render_queue.opaque_buckets.push_back(item);
			}
		}
	}
}