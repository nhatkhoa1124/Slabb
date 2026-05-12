#pragma once
#include "common/common_graphics.hpp"

using Microsoft::WRL::ComPtr;

class ID3D12CommandQueue;
class ID3D12Device;

namespace slabb::graphics::wrapper::command
{
	class SLABB_EXPORT CommandQueue
	{
	public:
		CommandQueue();
		void create_command_queue(ID3D12Device* device);

		[[nodiscard]] inline ID3D12CommandQueue* command_queue() const { return m_cmd_queue.Get(); }
	private:
		ComPtr<ID3D12CommandQueue> m_cmd_queue;
	};
}