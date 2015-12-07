#pragma once

#include <string>

namespace Common
{
	class EncodingConverter
	{
	public:
		EncodingConverter() = delete;
		EncodingConverter(const EncodingConverter &) = delete;

		static std::wstring s2ws(const std::string& s);
		static std::string ws2s(const std::wstring& s);
	};
}