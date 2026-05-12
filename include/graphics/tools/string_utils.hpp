#pragma once
#include <string>

namespace slabb::graphics::tools
{
	class StringUtils
	{
	public:
		static std::string convert_wchar_to_string(const wchar_t* w_char)
		{
			const std::wstring w_string(w_char);
			return std::string(w_string.begin(), w_string.end());
		}
	};

}