#pragma once
#include "common/common_graphics.hpp"
#include <memory>
#include <vector>
#include <string>

#include "../core/core_interface/core_vertex.hpp"

namespace slabb::graphics
{
	class TextureResource;
	class RenderGraph;
	class RenderPipeline;
	
	struct FrameContext;
	struct GraphicsModel; // Interface model
	struct RenderModel;   // GPU model
}

namespace slabb::graphics::wrapper
{
	class Device;
	class Instance;
	class Swapchain;
	class RootSignature;
}

namespace slabb::graphics::wrapper::command
{
	class CommandAllocator;
	class CommandList;
	class CommandQueue;
	class RootSignature;
}

namespace slabb::graphics::wrapper::descriptor
{
	class DescriptorHeap;
}

namespace slabb::graphics::wrapper::pipeline
{
	class GraphicsPipeline;
}

namespace slabb::graphics::wrapper::synchronization
{
	class Fence;
}

using Microsoft::WRL::ComPtr;
using slabb::graphics::wrapper::Device;
using slabb::graphics::wrapper::Instance;
using slabb::graphics::wrapper::Swapchain;
using slabb::graphics::wrapper::RootSignature;
using slabb::graphics::wrapper::command::CommandAllocator;
using slabb::graphics::wrapper::command::CommandList;
using slabb::graphics::wrapper::command::CommandQueue;
using slabb::graphics::wrapper::descriptor::DescriptorHeap;
using slabb::graphics::wrapper::pipeline::GraphicsPipeline;
using slabb::graphics::wrapper::synchronization::Fence;

struct ID3D12PipelineState;

namespace slabb::graphics
{
	class SLABB_EXPORT Renderer {
	public:
		Renderer(UINT window_width, UINT window_height);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		/**
		* @brief This method initialize core structures in the renderer
		* @param param The native window handler
		*/
		bool init_backend(HWND hWnd);

		/**
		* @brief This method initialize pipeline structures in the renderer
		* @param vertex_attributes The vertex attribute interface 
		*/
		bool init_pipeline(const std::string& vertex_path, const std::string& pixel_path,
							std::vector <core::VertexAttribute> vertex_attributes);
		void load_model(const GraphicsModel& model);
		void render_frame();

		// TODO: Move this into managed scene class
		void update_uniform_data(const FrameContext& frame);

	private:
		std::unique_ptr<RenderGraph> m_render_graph;
		std::vector<TextureResource*> m_graph_backbuffers;
		std::vector<RenderModel> m_scene_models;
		std::vector<FrameContext> m_frames;
		std::unique_ptr<RenderPipeline> m_render_pipeline;

		std::unique_ptr<Instance> m_instance;
		std::unique_ptr<Device> m_device;
		std::unique_ptr<Swapchain> m_swapchain;
		std::unique_ptr<CommandList> m_cmd_list;
		std::unique_ptr<CommandQueue> m_cmd_queue;
		std::unique_ptr<DescriptorHeap> m_descriptor_heap;
		std::unique_ptr<GraphicsPipeline> m_graphics_pipeline;
		std::unique_ptr<RootSignature> m_global_root_signature;
	};
}

