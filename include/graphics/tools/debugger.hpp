#pragma once
#include "common/common_graphics.hpp"
#include <dxgidebug.h>
#include <d3d12sdklayers.h>
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <comdef.h>

using Microsoft::WRL::ComPtr;

namespace slabb::graphics::tools {
	class SLABB_EXPORT Debugger
	{
	public:
		Debugger() = default;

		/**
		* @brief Enables D3D12 debug layer
		*/
		static void enable_debug_layer();

		/**
		* @brief Enables GPU-Based Validation
		* @param enable Use this to turn GBV on or off
		* @warning Call this only when debugging
		*/
		static void set_gbv(bool enable);
	};

#pragma region Macros
#ifndef SLABB_CHECK
	/**
	* @brief This macro checks if an HRESULT is false and throws an error 
	* @param hr The HRESULT returned by an operation
	*/
#define SLABB_CHECK(hr)														\
		HRESULT result = hr;												\
		if(FAILED(result))													\
		{																	\
			spdlog::critical("CHECK ERROR: {} | {}", __FILE__, __LINE__);	\
			throw std::runtime_error("ERROR: DirectX Failure");				\
		}																	\

#endif
#pragma endregion
}

