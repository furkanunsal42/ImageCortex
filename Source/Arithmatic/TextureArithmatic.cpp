#include "TextureArithmatic.h"
#include "ProgramSourcePaths.h"

void TextureArithmatic::operation_binary(Texture2D& source_texture, Texture2D& operand_texture, const std::string& shader_operator_symbol)
{
	_compile_shaders(_determine_preprocessor_defines<Texture2D, Texture2D>(
		source_texture.get_internal_format_color(),
		operand_texture.get_internal_format_color(),
		"int",
		true,
		shader_operator_symbol
		));

	ComputeProgram& kernel = *cp_texture_arithmatic;

	kernel.update_uniform_as_image("source_texture", source_texture, 0);
	kernel.update_uniform_as_image("operand_texture", operand_texture, 0);
	kernel.update_uniform("source_resolution", source_texture.get_size());
	kernel.update_uniform("operand_resolution", operand_texture.get_size());
	kernel.update_uniform("constant_operator", (int)1);

	kernel.dispatch_thread(source_texture.get_size().x, source_texture.get_size().y, 1);
}

template<typename operator_type>
void TextureArithmatic::operation_binary(Texture2D& source_texture, operator_type constant_operator, const std::string& shader_constant_operator_type_name, const std::string& shader_operator_symbol)
{
	_compile_shaders(_determine_preprocessor_defines<Texture2D, Texture2D>(
		source_texture.get_internal_format_color(),
		Texture2D::ColorTextureFormat::R8,
		shader_constant_operator_type_name,
		false,
		shader_operator_symbol
	));

	ComputeProgram& kernel = *cp_texture_arithmatic;

	kernel.update_uniform_as_image("source_texture", source_texture, 0);
	//kernel.update_uniform_as_image("operand_texture", operand_texture, 0);
	kernel.update_uniform("source_resolution", source_texture.get_size());
	//kernel.update_uniform("operand_resolution", operand_texture.get_size());
	kernel.update_uniform("constant_operator", (operator_type)constant_operator);

	kernel.dispatch_thread(source_texture.get_size().x, source_texture.get_size().y, 1);
}

template<typename source_texture_type, typename operand_texture_type>
std::vector<std::pair<std::string, std::string>> TextureArithmatic::_determine_preprocessor_defines(
	Texture2D::ColorTextureFormat source_format,
	Texture2D::ColorTextureFormat operand_format,
	const std::string& constant_operand_type,
	bool use_texture_operand,
	const std::string& operation)
{
	std::vector<std::pair<std::string, std::string>> preprocessors;

	preprocessors.push_back(std::pair<std::string, std::string>
		("source_image_internal_format", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_format(source_format)));
	preprocessors.push_back(std::pair<std::string, std::string>
		("source_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_type<source_texture_type>(source_format)));

	preprocessors.push_back(std::pair<std::string, std::string>
		("operand_image_internal_format", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_format(operand_format)));
	preprocessors.push_back(std::pair<std::string, std::string>
		("operand_image_type", TextureBase2::ColorTextureFormat_to_OpenGL_compute_Image_type<operand_texture_type>(operand_format)));
	preprocessors.push_back(std::pair<std::string, std::string>
		("operand_image_enabled", use_texture_operand ? "1" : "0"));

	preprocessors.push_back(std::pair<std::string, std::string>
		("constant_operand_type", constant_operand_type));
	preprocessors.push_back(std::pair<std::string, std::string>
		("operation", operation));

	//for (int i = 0; i < preprocessors.size(); i++)
	//	std::cout << preprocessors[i].first << " " << preprocessors[i].second << std::endl;

	return preprocessors;
}

void TextureArithmatic::operation_unary(Texture2D& source_texture, const std::string& shader_operator_symbol)
{
	operation_binary<int>(source_texture, 0, "int", shader_operator_symbol);
}

void TextureArithmatic::_compile_shaders(const std::vector<std::pair<std::string, std::string>>& arithmatic_preprocessing_defines)
{
	if (!_is_preprocessors_same(arithmatic_preprocessing_defines, _current_preprocessor_defines)) {
		_current_preprocessor_defines = arithmatic_preprocessing_defines;
		cp_texture_arithmatic = std::make_shared<ComputeProgram>(Shader(shader_directory::arithmatic_shader_directory + "texture_arithmatic.comp"), arithmatic_preprocessing_defines);
	}
}


bool TextureArithmatic::_is_preprocessors_same(
	const std::vector<std::pair<std::string, std::string>>& preprocessors_A, 
	const std::vector<std::pair<std::string, std::string>>& preprocessors_B)
{
	if (preprocessors_A.size() != preprocessors_B.size()) return false;
	int size = preprocessors_A.size();

	for (int i = 0; i < size; i++)
		if (preprocessors_A[i] != preprocessors_B[i]) return false;

	return true;
}

void TextureArithmatic::add(Texture2D& source_texture, Texture2D& operand_texture)
{
	operation_binary(source_texture, operand_texture, "source+operand");
}

void TextureArithmatic::sub(Texture2D& source_texture, Texture2D& operand_texture)
{
	operation_binary(source_texture, operand_texture, "source-operand");
}

void TextureArithmatic::sub_inverse(Texture2D& source_texture, Texture2D& operand_texture)
{
	operation_binary(source_texture, operand_texture, "operand-source");
}

void TextureArithmatic::mult(Texture2D& source_texture, Texture2D& operand_texture)
{
	operation_binary(source_texture, operand_texture, "source*operand");
}

void TextureArithmatic::div(Texture2D& source_texture, Texture2D& operand_texture)
{
	operation_binary(source_texture, operand_texture, "source/operand");
}

void TextureArithmatic::div_inverse(Texture2D& source_texture, Texture2D& operand_texture)
{
	operation_binary(source_texture, operand_texture, "operand/source");
}
