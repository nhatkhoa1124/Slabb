#include "graphics/wrapper/synchronization/fence.hpp"

#include "graphics/tools/debug.hpp"

namespace slabb::graphics::wrapper::synchronization
{
	Fence::Fence() : 
		m_fence_value{ 1 }, m_fence_event{ nullptr }
	{}

	void Fence::create_fence(ID3D12Device* device)
	{
		NULL_CHECK(device);
		spdlog::info("Creating fence...");
		SLABB_CHECK(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fence_value = 1;
		create_fence_event();
		spdlog::info("Fence created successfully");
	}

	void Fence::create_fence_event()
	{
		spdlog::info("Creating fence event...");
		m_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fence_event == nullptr)
		{
			spdlog::error("Fence event creation failed");
			SLABB_CHECK(HRESULT_FROM_WIN32(GetLastError()));
		}
		spdlog::info("Fence event created successfully");
	}

	void Fence::wait_for_fence(ID3D12CommandQueue* cmd_queue)
	{
		m_fence_value++;
		cmd_queue->Signal(m_fence.Get(), m_fence_value);
		if (m_fence->GetCompletedValue() < m_fence_value)
		{
			m_fence->SetEventOnCompletion(m_fence_value, m_fence_event);
			WaitForSingleObject(m_fence_event, INFINITE); // Waits indefinitely on CPU
		}
	}
}