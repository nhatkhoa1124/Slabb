#pragma once
#include "common/common_graphics.hpp"
#include <memory>

#include "instance.hpp"

using Microsoft::WRL::ComPtr;

class ID3D12Device;

namespace slabb::graphics::wrapper
{
	class SLABB_EXPORT Device {
	public:
		Device();
		/**
		* @brief Create D3D12 virtual device
		*/
		void create_device();

		[[nodiscard]] inline ID3D12Device* device() const { return m_device.Get(); }
		[[nodiscard]] inline Instance* instance() const { return m_instance.get(); }
	private:
		std::unique_ptr<Instance> m_instance;
		ComPtr<ID3D12Device> m_device;
	};
}
