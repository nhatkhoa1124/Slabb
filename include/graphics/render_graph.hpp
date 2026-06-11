#pragma once
#include "common/common_graphics.hpp"
#include <directx/d3d12.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>

#include "wrapper/instance.hpp"
#include "wrapper/command/command_list.hpp"

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

		[[nodiscard]] const std::string& name() const { return m_name; }
	private:
		const std::string m_name;
	};

	/**
	* @brief D3D12 hardware-specific data for buffer resource
	*/
	struct BufferHardware
	{
		ComPtr<ID3D12Resource> resource{ nullptr };
		D3D12_VERTEX_BUFFER_VIEW view{};
	};

	// TODO: Add Index Buffer support
	/**
	* @brief Buffer class handles CPU side buffer works
	*/
	class SLABB_EXPORT BufferResource : public RenderResource
	{
	public:
		explicit BufferResource(std::string name) : RenderResource{std::move(name)} {};

		template <typename T>
		void upload_data(const T* data, std::size_t element_count)
		{
			// Deep copy to avoid lifetime issues
			const auto* bytes = reinterpret_cast<const std::byte*>(data);
			m_data.assign(bytes, bytes + sizeof(T) * element_count);
		}

		[[nodiscard]] const BufferHardware& hardware() const { return m_hardware; }

	private:
		std::vector<std::byte> m_data{};

		BufferHardware m_hardware{};
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

		[[nodiscard]] const std::vector<const RenderResource*>& write_resources() const { return m_writes; }
		[[nodiscard]] const std::vector<const RenderResource*>& read_resources() const { return m_reads; }

		D3D12_VIEWPORT viewport{};
		D3D12_RECT rect{};
	private:
		const std::string m_name;

		std::vector<const RenderResource*> m_writes;
		std::vector<const RenderResource*> m_reads;
		std::function<void(wrapper::command::CommandList&)> m_callback;
	};

	class SLABB_EXPORT RenderGraph {
	public:
		RenderGraph(int width, int height);

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
			m_resources.push_back(std::move(resource));
			return raw_ptr;
		}

		void compile();

		// TODO: Temporary method, remove this later
		[[nodiscard]] const RenderPass& render_pass() const { return m_render_pass; }

	private:

		/**
		* @brief Returns the index of the resource written to by a render pass
		*/
		size_t find_producer(const RenderResource* resource);

		/**
		* @brief Returns a list of resources to be read by a render pass
		*/
		std::vector<size_t> find_consumers(const RenderResource* resource);

	private:
		// TODO: Temporary member, remove this later
		RenderPass m_render_pass; 

		std::vector<std::unique_ptr<RenderResource>> m_resources;
		std::vector<std::unique_ptr<RenderPass>> m_render_passes;
		std::vector<size_t> m_execution_queue;
	};
}
