#pragma once
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <comdef.h>

namespace slabb::graphics::tools
{
#ifndef SLABB_CHECK
	/**
	* @brief This macro checks if an HRESULT is false and throws an error
	* @param hr The HRESULT returned by an operation
	*/
#define SLABB_CHECK(hr)															\
	{																			\
		HRESULT result = hr;													\
		if (FAILED(result))														\
		{																		\
			spdlog::critical("SLABB CHECK ERROR: {} | {}", __FILE__, __LINE__);	\
			throw std::runtime_error("ERROR: DirectX Failure");					\
		}																		\
	}																			\

#endif
}

