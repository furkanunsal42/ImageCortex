#pragma once

#include <vector>
#include <string>

const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f16 =
{
	{ "ffft_complex_format", "rg16f" },
	{ "ffft_real_format", "r16f" },
	{ "horizontal", "1" },
};


const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f16_h =
{
	{ "ffft_complex_format", "rg16f" },
	{ "ffft_real_format", "r16f" },
	{ "horizontal", "1" },
};

const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f16_v =
{
	{ "ffft_complex_format", "rg16f" },
	{ "ffft_real_format", "r16f" },
	{ "horizontal", "0" },
};

const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f32 =
{
	{ "ffft_complex_format", "rg32f" },
	{ "ffft_real_format", "r32f" },
	{ "horizontal", "1" },
};

const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f32_h =
{
	{ "ffft_complex_format", "rg32f" },
	{ "ffft_real_format", "r32f" },
	{ "horizontal", "1" },
};

const std::vector<std::pair<std::string, std::string>> _ffft_shader_defines_f32_v =
{
	{ "ffft_complex_format", "rg32f" },
	{ "ffft_real_format", "r32f" },
	{ "horizontal", "0" },
};

const std::vector<std::pair<std::string, std::string>> ffft_shader_defines = _ffft_shader_defines_f16;