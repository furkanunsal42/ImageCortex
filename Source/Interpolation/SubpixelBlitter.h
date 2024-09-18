#pragma once
#include <memory>
#include <vector>
#include <string>
#include "glm.hpp"

#include "Texture2D.h"

class ComputeProgram;

class SubpixelBlitter {
public:
	SubpixelBlitter(SubpixelBlitter& other) = delete;
	SubpixelBlitter() = default;

	void blit(Texture2D& source, Texture2D& target);
	void blit(Texture2D& source, Texture2D& target, glm::vec2 begin_pixels, glm::vec2 end_pixels);
	
	std::shared_ptr<Texture2D> blit(Texture2D& source, glm::ivec2 resolution);
	std::shared_ptr<Texture2D> blit(Texture2D& source, glm::ivec2 resolution, glm::vec2 begin_pixels, glm::vec2 end_pixels);

private:

	void _compile_shaders(const std::vector<std::pair<std::string, std::string>>& blitter_preprocessing_defines);

	template<typename source_texture_type, typename target_texture_type>
	std::vector<std::pair<std::string, std::string>> _determine_preprocessor_defines(
		Texture2D::ColorTextureFormat source_format,
		Texture2D::ColorTextureFormat target_format,
		bool use_sampler_for_source);

	bool _is_preprocessors_same(
		const std::vector<std::pair<std::string, std::string>>& preprocessors_A, 
		const std::vector<std::pair<std::string, std::string>>& preprocessors_B);

	std::shared_ptr<ComputeProgram> cp_texture_blit;
	
	std::vector<std::pair<std::string, std::string>> _current_preprocessor_defines;
};