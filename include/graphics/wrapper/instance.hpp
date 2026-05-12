#pragma once 
#include "common/common_graphics.hpp"
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::wrapper
{
	/**
	* @brief Wrapper class for D3D12 factory and adapter operations
	*/
	class SLABB_EXPORT Instance
	{
	public:
		Instance() = default;

		/**
		* @brief Creates a factory for physical hardware management
		*/
		void create_factory();

		/**
		* @brief Get the best adapter on current machine
		*/
		void enumerate_adapters();

		inline IDXGIFactory4* factory() const { return m_factory.Get(); }
		inline IDXGIAdapter4* adapter() const { return m_adapter.Get(); }

	private:
		ComPtr<IDXGIFactory4> m_factory;
		ComPtr<IDXGIAdapter4> m_adapter;
	};
}