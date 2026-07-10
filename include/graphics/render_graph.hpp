#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <DirectXMath.h>

#include "wrapper/instance.hpp"
#include "wrapper/command/command_list.hpp"
#include "wrapper/resource/resource_heap.hpp"

using Microsoft::WRL::ComPtr;

namespace slabb::graphics
{
	/**
	*@brief Base class for render graph's render resource
	*/
	class SLABB_EXPORT RenderResource {
	public:
		explicit RenderResource(std::string name) : m_name{std::move(name)} {};
		virtual ~RenderResource() = default;
		void set_state(D3D12_RESOURCE_STATES resource_state) { m_current_state = resource_state; }

		/**
		* @brief Set underlying D3D12 resource
		*/
		virtual void set_native_resource(ID3D12Resource* resource) = 0;

		[[nodiscard]] const std::string& name() const { return m_name; }
		[[nodiscard]] D3D12_RESOURCE_STATES current_state() const { return m_current_state; }
		[[nodiscard]] virtual ID3D12Resource* underlying_resource() const = 0;
	private:
		const std::string m_name;
		D3D12_RESOURCE_STATES m_current_state{D3D12_RESOURCE_STATE_COMMON};
	};

	enum class BufferUsage
	{
		VERTEX,
		INDEX
	};

	/**
	* @brief Buffer class handles CPU side buffer works
	*/
	class SLABB_EXPORT BufferResource : public RenderResource
	{
	public:
		explicit BufferResource(std::string name, BufferUsage buffer_usage = BufferUsage::VERTEX) :
			RenderResource{ std::move(name) }, m_usage{ buffer_usage } {};

		/**
		* @brief Copy data to CPU's staging buffer to prepare for GPU uploading
		*/
		void stage_data(const void* data, std::size_t total_bytes)
		{
			if (!data || total_bytes == 0) return;
			const auto* bytes = reinterpret_cast<const std::byte*>(data);

			// Wipe the vector and safely copy the exact size needed
			m_data.assign(bytes, bytes + total_bytes);
		}

		void set_native_resource(ID3D12Resource* resource) override {}

		/**
		* @brief Initialize hardwares for BufferResource
		* @param device Pointer to DX12 device
		* @param stride_in_bytes Size between each elements in the data array
		* @param index_format Format for index buffer, leave unspecified for vertex buffers
		*/
		void initialize_hardware(ID3D12Device* device, UINT stride_in_bytes, DXGI_FORMAT index_format = DXGI_FORMAT_UNKNOWN);

		[[nodiscard]] ID3D12Resource* underlying_resource() const override { return m_hardware_heap ? m_hardware_heap->resource_heap() : nullptr; }
		[[nodiscard]] BufferUsage usage() const { return m_usage; }
		[[nodiscard]] const D3D12_VERTEX_BUFFER_VIEW& vertex_view() const { return m_vertex_view; }
		[[nodiscard]] const D3D12_INDEX_BUFFER_VIEW& index_view() const { return m_index_view; }
		[[nodiscard]] D3D12_HEAP_TYPE hardware_heap_type() const { return m_hardware_heap->heap_type(); }

	private:
		BufferUsage m_usage;
		std::vector<std::byte> m_data{};
		std::unique_ptr<wrapper::resource::BufferHeap> m_hardware_heap{ nullptr };

		D3D12_VERTEX_BUFFER_VIEW m_vertex_view{};
		D3D12_INDEX_BUFFER_VIEW m_index_view{};
	};

	struct RenderMesh
	{
		// Graph resource tracking handles
		BufferResource* vertex_buffer{ nullptr };
		BufferResource* index_buffer{ nullptr }; // Ready for when you add indices!

		uint32_t vertex_count{ 0 };
		uint32_t index_count{ 0 };
	};

	struct RenderModel
	{
		std::vector<RenderMesh> sub_meshes;
		DirectX::XMMATRIX transform;
	};

	/**
	* @brief Enum specifying type of texture
	*/
	enum class TextureUsage
	{
		BACK_BUFFER,
		DEPTH_STENCIL_BUFFER,
		COMMON
	};

	class SLABB_EXPORT TextureResource : public RenderResource
	{
	public:
		explicit TextureResource(std::string name, TextureUsage usage = TextureUsage::COMMON)
			: RenderResource{ std::move(name) }, m_usage{ usage } {};
		void set_native_resource(ID3D12Resource* resource) override { m_resource = resource; }

		[[nodiscard]] TextureUsage usage() const { return m_usage; }
		[[nodiscard]] ID3D12Resource* underlying_resource() const override { return m_resource.Get(); }
	private:
		TextureUsage m_usage;
		ComPtr<ID3D12Resource> m_resource{nullptr};
	};

	/**
	* @brief This class represents a render pass in the render graph
	*/
	class SLABB_EXPORT RenderPass {
	public:
		explicit RenderPass(std::string name) : m_name{std::move(name)} {};

		void writes_to(const RenderResource* resource);
		void reads_from(const RenderResource* resource);
		void record(std::function<void(wrapper::command::CommandList&)> callback);
		void execute(wrapper::command::CommandList& cmd_list);

		void set_viewport(const D3D12_VIEWPORT& viewport) { m_viewport = viewport; }
		void set_rect(const D3D12_RECT& rect) { m_rect = rect; }
		void set_pipeline_state(ID3D12PipelineState* pso) { m_pso = pso; }
		void set_root_signature(ID3D12RootSignature* root_signature) { m_root_signature = root_signature; }
		void add_resource_barrier(const D3D12_RESOURCE_BARRIER& barrier) { m_barriers.push_back(barrier); }
		void clear_barriers() { m_barriers.clear(); }

		[[nodiscard]] const std::vector<const RenderResource*>& write_resources() const { return m_writes; }
		[[nodiscard]] const std::vector<const RenderResource*>& read_resources() const { return m_reads; }
		[[nodiscard]] const D3D12_VIEWPORT& viewport() const { return m_viewport; }
		[[nodiscard]] const D3D12_RECT& rect() const { return m_rect; }

	private:
		const std::string m_name;

		std::vector<const RenderResource*> m_writes;
		std::vector<const RenderResource*> m_reads;
		std::vector<D3D12_RESOURCE_BARRIER> m_barriers;

		std::function<void(wrapper::command::CommandList&)> m_callback;
		D3D12_VIEWPORT m_viewport{};
		D3D12_RECT m_rect{};
		ID3D12PipelineState* m_pso{ nullptr };
		ID3D12RootSignature* m_root_signature{ nullptr };
	};

	class SLABB_EXPORT RenderGraph {
	public:
		RenderGraph();
		~RenderGraph() = default;

		RenderGraph(const RenderGraph&) = delete;
		RenderGraph& operator=(const RenderGraph&) = delete;
		RenderGraph(RenderGraph&&) noexcept = default;
		RenderGraph& operator=(RenderGraph&&) noexcept = default;

		/**
		* @brief Adds a new render pass to the graph
		*/
		RenderPass& add_pass(std::string name);

		/**
		* @brief Creates a new resource for the passes to consumes
		*/
		template <typename ResourceType, typename... Args>
		[[nodiscard]] ResourceType* create_resource(Args&&... args)
		{
			auto resource = std::make_unique<ResourceType>(std::forward<Args>(args)...);
			auto* raw_ptr = resource.get();
			std::unique_ptr<RenderResource> base_resource = std::move(resource);
			m_resources.push_back(std::move(base_resource));
			return raw_ptr;
		}

		void compile();
		void render(wrapper::command::CommandList& cmd_list);
		void clear();
	private:

		/**
		* @brief Returns the index of the resource written to by a render pass
		*/
		size_t find_producer(const RenderResource* resource);

		/**
		* @brief Returns a list of resources to be read by a render pass
		*/
		std::vector<size_t> find_consumers(const RenderResource* resource);

		/**
		* @brief Handles resource barrier transitions
		*/
		void build_resource_barriers();

		/**
		* @brief Determine the valid state for barrier transitioning
		* @param resource The resource to evaluate
		* @param is_writer Resource is write or read
		*/
		D3D12_RESOURCE_STATES evalute_state(const RenderResource* resource, bool is_writer);

	private:
		std::vector<std::unique_ptr<RenderResource>> m_resources;
		std::vector<std::unique_ptr<RenderPass>> m_render_passes;
		std::vector<size_t> m_execution_queue;
	};
}
