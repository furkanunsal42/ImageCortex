#include "GraphicsCortex.h"
#include "FastFourierTransform/FastFourierTransform.h"
#include "Arithmatic/TextureArithmatic.h"

int main() {

	Frame frame(0, 0, "ImageCortex", 0, 0, true, true, false, Frame::NOTIFICATION, false);
	Scene scene(frame);

	std::shared_ptr<Image> image = std::make_shared<Image>("../ImageCortex/Images/0_3m_gsd_image.png", 3, true);
	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(*image,
		Texture2D::ColorTextureFormat::R32F, Texture2D::ColorFormat::RGB, Texture2D::Type::UNSIGNED_BYTE, 1, 0, 0);
	
	TextureArithmatic arithmatic;

	FFFT fft_solver;
	fft_solver.set_complex_format(Texture2D::ColorTextureFormat::RG32F);
	fft_solver.set_real_format(Texture2D::ColorTextureFormat::R32F);

	fft_solver.compile_shaders(_ffft_shader_defines_f32_v);
	fft_solver.set_axis(FFFT::VERTICAL);

	std::shared_ptr<Texture2D> complex_texture = fft_solver.create_complex_texture_from_real(*texture);
	std::shared_ptr<Texture2D> fourier_space_horizontal = fft_solver.fft(*complex_texture);
	fft_solver.fft_shift(*fourier_space_horizontal);

	fft_solver.compile_shaders(_ffft_shader_defines_f32_h);
	fft_solver.set_axis(FFFT::HORIZONTAL);

	std::shared_ptr<Texture2D> fourier_space = fft_solver.fft(*fourier_space_horizontal);
	fft_solver.fft_shift(*fourier_space);

	arithmatic.operation_unary(*fourier_space, "length(source) < 100000 ? source : vec4(0)");

	fft_solver.compile_shaders(_ffft_shader_defines_f32_h);
	fft_solver.set_axis(FFFT::HORIZONTAL);

	fft_solver.inverse_fft_shift(*fourier_space);
	fft_solver.inverse_fft(*fourier_space, *fourier_space_horizontal);

	fft_solver.compile_shaders(_ffft_shader_defines_f32_v);
	fft_solver.set_axis(FFFT::VERTICAL);

	fft_solver.inverse_fft_shift(*fourier_space_horizontal);
	fft_solver.inverse_fft(*fourier_space_horizontal, *complex_texture);

	std::shared_ptr<Texture2D> lowpass_texture = texture->create_texture_with_same_parameters();
	fft_solver.blit_texture_complex_to_real_r(*complex_texture, *lowpass_texture);

	arithmatic.operation_unary(*lowpass_texture, "max(source, vec4(0.1))");

	arithmatic.div(*texture, *lowpass_texture);
	std::shared_ptr<Texture2D> texture_white = std::make_shared<Texture2D>(texture->get_size().x, texture->get_size().y, Texture2D::ColorTextureFormat::RGBA32F, 1, 0, 0);
	arithmatic.operation_binary(*texture_white, *texture, "vec4(operand.xxx, 1)");

	arithmatic.operation_unary(*texture_white, "source/2");

	Framebuffer framebuffer;
	framebuffer.attach_color(0, texture_white);

	frame.resize(texture_white->get_size().x, texture_white->get_size().y);
	frame.set_visibility(true);
	while (frame.is_running()) {
		double deltatime = frame.handle_window();
		frame.clear_window();
		frame.display_performance();

		framebuffer.set_read_buffer(0);
		framebuffer.blit_to_screen(0, 0, texture_white->get_size().x, texture_white->get_size().y, 0, 0, frame.window_width, frame.window_height, Framebuffer::Channel::COLOR, Framebuffer::Filter::LINEAR);
	}
}