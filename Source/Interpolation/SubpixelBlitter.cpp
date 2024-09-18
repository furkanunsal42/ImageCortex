#include "SubpixelBlitter.h"
#include "ProgramSourcePaths.h"

#include "ComputeProgram.h"

void SubpixelBlitter::blit(Texture2D& source, Texture2D& target, bool use_interpolation)
{
	blit(source, target, glm::vec2(0, 0), source.get_size(), use_interpolation);
}

void SubpixelBlitter::blit(Texture2D& source, Texture2D& target, glm::vec2 begin_pixels, glm::vec2 end_pixels, bool use_interpolation)
{
	_compile_shaders(_determine_preprocessor_defines<Texture2D, Texture2D>(
		source.get_internal_format_color(),
		target.get_internal_format_color(), 
		Texture2D::ColorTextureFormat::R32UI,
		use_interpolation, 
		true, 
		false));

	ComputeProgram& kernel = *cp_texture_blit;

	if (use_interpolation)	kernel.update_uniform("source_texture", source);
	else					kernel.update_uniform_as_image("source_texture", source, 0);
	kernel.update_uniform_as_image("target_texture", target, 0);
	kernel.update_uniform("source_resolution", source.get_size());
	kernel.update_uniform("target_resolution", target.get_size());
	kernel.update_uniform("target_begin", begin_pixels);
	kernel.update_uniform("target_end", end_pixels);

	kernel.dispatch_thread(target.get_size().x, target.get_size().y, 1);
}

std::shared_ptr<Texture2D> SubpixelBlitter::blit(Texture2D& source, glm::ivec2 resolution, bool use_interpolation)
{
	std::shared_ptr<Texture2D> target = std::make_shared<Texture2D>(resolution.x, resolution.y, source.get_internal_format_color(), 1, 0, 0);
	
	blit(source, *target, use_interpolation);

	return target;
}

std::shared_ptr<Texture2D> SubpixelBlitter::blit(Texture2D& source, glm::ivec2 resolution, glm::vec2 begin_pixels, glm::vec2 end_pixels, bool use_interpolation)
{
	std::shared_ptr<Texture2D> target = std::make_shared<Texture2D>(resolution.x, resolution.y, source.get_internal_format_color(), 1, 0, 0);

	blit(source, *target, begin_pixels, end_pixels, use_interpolation);

	return target;
}

void SubpixelBlitter::merge(std::vector<std::reference_wrapper<Texture2D>>& sources, Texture2D& target, std::vector<glm::vec2> begin_pixels, std::vector<glm::vec2> end_pixels)
{
	bool use_interpolation = true;
	std::shared_ptr<Texture2D> stencil = std::make_shared<Texture2D>(target.get_size().x, target.get_size().y, stencil_internal_format, 1, 0, 0);

	for (int i = 0; i < sources.size(); i++) {
		Texture2D& source = sources[i];
		glm::vec2& begin_pixel = begin_pixels[i];
		glm::vec2& end_pixel = end_pixels[i];

		_compile_shaders(_determine_preprocessor_defines<Texture2D, Texture2D>(
			source.get_internal_format_color(),
			target.get_internal_format_color(),
			stencil_internal_format,
			use_interpolation,
			false,
			true));

		ComputeProgram& kernel = *cp_texture_blit;

		if (use_interpolation)	kernel.update_uniform("source_texture", source);
		else					kernel.update_uniform_as_image("source_texture", source, 0);
		kernel.update_uniform_as_image("target_texture", target, 0);
		kernel.update_uniform_as_image("stencil_texture", *stencil, 0);
		kernel.update_uniform("source_resolution", source.get_size());
		kernel.update_uniform("target_resolution", target.get_size());
		kernel.update_uniform("target_begin", begin_pixel);
		kernel.update_uniform("target_end", end_pixel);

		kernel.dispatch_thread(target.get_size().x, target.get_size().y, 1);
	}
}

void SubpixelBlitter::_compile_shaders(
	const std::vector<std::pair<std::string, std::string>>& preprocessing_defines)
{
	if (!_is_preprocessors_same(preprocessing_defines, _current_preprocessor_defines)) {
		_current_preprocessor_defines = preprocessing_defines;
		cp_texture_blit = std::make_shared<ComputeProgram>(Shader(shader_directory::blitter_shader_directory + "texture_blit.comp"), preprocessing_defines);
	}

}

template<typename source_texture_type, typename target_texture_type>
std::vector<std::pair<std::string, std::string>> SubpixelBlitter::_determine_preprocessor_defines(
	Texture2D::ColorTextureFormat source_format,
	Texture2D::ColorTextureFormat target_format,
	Texture2D::ColorTextureFormat stencil_format,
	bool use_sampler_for_source,
	bool target_texture_overwrite,
	bool stencil_texture_enabled)
{
	std::vector<std::pair<std::string, std::string>> preprocessors;

	preprocessors.push_back(std::pair<std::string, std::string>
		("source_image_internal_format", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_format(source_format)));
	
	if (use_sampler_for_source)
	{
		preprocessors.push_back(std::pair<std::string, std::string>
			("source_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Sampler_type<source_texture_type>()));
		preprocessors.push_back(std::pair<std::string, std::string>
			("use_sampler_for_source", "1"));
	}
	else {
		preprocessors.push_back(std::pair<std::string, std::string>
			("source_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_type<source_texture_type>(source_format)));
		preprocessors.push_back(std::pair<std::string, std::string>
			("use_sampler_for_source", "0"));
	}
	
	preprocessors.push_back(std::pair<std::string, std::string>
		("target_image_internal_format", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_format(target_format)));
	preprocessors.push_back(std::pair<std::string, std::string>
		("target_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_type<target_texture_type>(target_format)));
	
	preprocessors.push_back(std::pair<std::string, std::string>
		("target_image_overwrite", target_texture_overwrite ? "1" : "0"));
	preprocessors.push_back(std::pair<std::string, std::string>
		("stencil_image_internal_format", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_format(stencil_format)));
	preprocessors.push_back(std::pair<std::string, std::string>
		("stencil_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_type<target_texture_type>(stencil_format)));
	preprocessors.push_back(std::pair<std::string, std::string>
		("stencil_image_enabled", stencil_texture_enabled ? "1" : "0"));

	for (int i = 0; i < preprocessors.size(); i++)
		std::cout << preprocessors[i].first << " " << preprocessors[i].second << std::endl;

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