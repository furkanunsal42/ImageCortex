#pragma once
#include <string>
#include <vector>
#include "Texture2D.h"
#include "ComputeProgram.h"

class TextureArithmatic {
public:

	TextureArithmatic(TextureArithmatic& other) = delete;
	TextureArithmatic() = default;

	void add(Texture2D& source_texture, Texture2D& operand_texture);
	void sub(Texture2D& source_texture, Texture2D& operand_texture);
	void sub_inverse(Texture2D& source_texture, Texture2D& operand_texture);
	void mult(Texture2D& source_texture, Texture2D& operand_texture);
	void div(Texture2D& source_texture, Texture2D& operand_texture);
	void div_inverse(Texture2D& source_texture, Texture2D& operand_texture);

	void operation_binary(Texture2D& source_texture, Texture2D& operand_texture,	const std::string& shader_operator_symbol);
	template<typename operator_type>
	void operation_binary(Texture2D& source_texture, operator_type constant_operator, const std::string& shader_constant_operator_type_name, const std::string& shader_operator_symbol);

	void operation_unary(Texture2D& source_texture, const std::string& shader_operator_symbol);

private:

	void _compile_shaders(const std::vector<std::pair<std::string, std::string>>& arithmatic_preprocessing_defines);

	template<typename source_texture_type, typename operand_texture_type>
	std::vector<std::pair<std::string, std::string>> _determine_preprocessor_defines(
		Texture2D::ColorTextureFormat source_format,
		Texture2D::ColorTextureFormat operand_format,
		const std::string& constant_operand_type,
		bool use_texture_operand,
		const std::string& operation);

	bool _is_preprocessors_same(
		const std::vector<std::pair<std::string, std::string>>& preprocessors_A,
		const std::vector<std::pair<std::string, std::string>>& preprocessors_B);

	std::shared_ptr<ComputeProgram> cp_texture_arithmatic;

	std::vector<std::pair<std::string, std::string>> _current_preprocessor_defines;
};
