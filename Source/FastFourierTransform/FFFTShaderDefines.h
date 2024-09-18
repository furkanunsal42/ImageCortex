#pragma once

#include <vector>
#include <string>

const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f16 =
{
	{ "ffft_complex_format", "rg16f" },
	{ "ffft_real_format", "r16f" },
};

const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f32 =
{
	{ "ffft_complex_format", "rg32f" },
	{ "ffft_real_format", "r32f" },
};

const std::vector<std::pair<std::string, std::string>> ffft_shader_defines = _ffft_shader_defines_f16;