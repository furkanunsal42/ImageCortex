#include "GraphicsCortex.h"
#include "FastFourierTransform/FastFourierTransform.h"

int main() {

	Frame frame(0, 0, "ImageCortex", 0, 0, true, true, false, Frame::NOTIFICATION, false);
	Scene scene(frame);

	std::shared_ptr<Image> image = std::make_shared<Image>("../ImageCortex/Images/0_3m_gsd_image.png", 3, true);
	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(*image,
		Texture2D::ColorTextureFormat::R32F, Texture2D::ColorFormat::RGB, Texture2D::Type::UNSIGNED_BYTE, 1, 0, 0);

	
	FFFT fft_solver;
	fft_solver.compile_shaders(_ffft_shader_defines_f32);
	fft_solver.set_complex_format(Texture2D::ColorTextureFormat::RG32F);
	fft_solver.set_real_format(Texture2D::ColorTextureFormat::R32F);

	std::shared_ptr<Texture2D> complex_texture = fft_solver.create_complex_texture_from_real(*texture);
	std::shared_ptr<Texture2D> fourier_space = fft_solver.fft(*complex_texture);

	Framebuffer framebuffer;
	framebuffer.attach_color(0, fourier_space);

	frame.resize(texture->get_size().x, texture->get_size().y);
	frame.set_visibility(true);
	while (frame.is_running()) {
		double deltatime = frame.handle_window();
		frame.clear_window();
		frame.display_performance();

		framebuffer.set_read_buffer(0);
		framebuffer.blit_to_screen(0, 0, texture->get_size().x, texture->get_size().y, 0, 0, frame.window_width, frame.window_height, Framebuffer::Channel::COLOR, Framebuffer::Filter::LINEAR);
	}
}