#include "SubpixelBlitter.h"
#include "ProgramSourcePaths.h"

#include "ComputeProgram.h"

void SubpixelBlitter::blit(Texture2D& source, Texture2D& target, glm::vec2 begin_pixels, glm::vec2 end_pixels)
{
	bool use_sampler_for_source = true;
	_compile_shaders(_determine_preprocessor_defines<Texture2D, Texture2D>(source.get_internal_format_color(), target.get_internal_format_color(), use_sampler_for_source));

	ComputeProgram& kernel = *cp_texture_blit;

	if (use_sampler_for_source) kernel.update_uniform("source_texture", source);
	else kernel.update_uniform_as_image("source_texture", source, 0);
	kernel.update_uniform_as_image("target_texture", target, 0);
	kernel.update_uniform("source_resolution", source.get_size());
	kernel.update_uniform("target_resolution", target.get_size());
	kernel.update_uniform("target_begin", begin_pixels);
	kernel.update_uniform("target_end", end_pixels);

	kernel.dispatch_thread(target.get_size().x, target.get_size().y, 1);
}

void SubpixelBlitter::_compile_shaders(const std::vector<std::pair<std::string, std::string>>& blitter_preprocessing_defines)
{
	if (_is_preprocessors_same(blitter_preprocessing_defines, _current_preprocessor_defines)) return;
	_current_preprocessor_defines = blitter_preprocessing_defines;

	cp_texture_blit = std::make_shared<ComputeProgram>(Shader(shader_directory::blitter_shader_directory + "texture_blit.comp"), blitter_preprocessing_defines);
}

template<typename source_texture_type, typename target_texture_type>
std::vector<std::pair<std::string, std::string>> SubpixelBlitter::_determine_preprocessor_defines(Texture2D::ColorTextureFormat source_format, Texture2D::ColorTextureFormat target_format, bool use_sampler_for_source)
{
	std::vector<std::pair<std::string, std::string>> preprocessors;

	preprocessors.push_back(std::pair<std::string, std::string>
		("source_image_internal_format", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_format(source_format)));
	
	if (use_sampler_for_source)
	{
		preprocessors.push_back(std::pair<std::string, std::string>
			("source_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Sampler_type<source_texture_type>()));
		preprocessors.push_back(std::pair<std::string, std::string>
			("use_sampler_for_source", "true"));
	}
	else {
		preprocessors.push_back(std::pair<std::string, std::string>
			("source_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_type<source_texture_type>(source_format)));
		preprocessors.push_back(std::pair<std::string, std::string>
			("use_sampler_for_source", "false"));
	}
	
	preprocessors.push_back(std::pair<std::string, std::string>
		("target_image_internal_format", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_format(target_format)));
	preprocessors.push_back(std::pair<std::string, std::string>
		("target_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_type<target_texture_type>(target_format)));

	return preprocessors;
}

bool SubpixelBlitter::_is_preprocessors_same(
	const std::vector<std::pair<std::string, std::string>>& preprocessors_A, 
	const std::vector<std::pair<std::string, std::string>>& preprocessors_B)
{
	if (preprocessors_A.size() != preprocessors_B.size()) return false;
	int size = preprocessors_A.size();

	for (int i = 0; i < size; i++)
		if (preprocessors_A[i] != preprocessors_B[i]) return false;

	return true;
}
