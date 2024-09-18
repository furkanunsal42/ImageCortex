#include "GraphicsCortex.h"
#include "Interpolation/SubpixelBlitter.h"

int main() {

	Frame frame(0, 0, "ImageCortex", 0, 0, true, true, false, Frame::NOTIFICATION, false);
	Scene scene(frame);

	std::shared_ptr<Image> image = std::make_shared<Image>("../ImageCortex/Images/0_3m_gsd_image.png", 3, true);
	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(*image,
		Texture2D::ColorTextureFormat::RGBA8, Texture2D::ColorFormat::RGB, Texture2D::Type::UNSIGNED_BYTE, 1, 0, 0);

	std::shared_ptr<Texture2D> target_texture_0 = std::make_shared<Texture2D>(texture->get_size().x / 2, texture->get_size().y / 2, Texture2D::ColorTextureFormat::RGBA8, 1, 0, 0);
	std::shared_ptr<Texture2D> target_texture_1 = std::make_shared<Texture2D>(texture->get_size().x / 2, texture->get_size().y / 2, Texture2D::ColorTextureFormat::RGBA8, 1, 0, 0);

	SubpixelBlitter blitter;

	blitter.blit(*texture, *target_texture_0, glm::vec2(0.25, 0.25), glm::vec2(texture->get_size()) + glm::vec2(0.25, 0.25));
	blitter.blit(*texture, *target_texture_1, glm::vec2(0.75, 0.75), glm::vec2(texture->get_size()) + glm::vec2(0.75, 0.75));

	Framebuffer framebuffer;
	framebuffer.attach_color(0, target_texture_0);
	framebuffer.attach_color(1, target_texture_1);

	int display_texture_index = 0;
	
	frame.resize(target_texture_0->get_size().x, target_texture_0->get_size().y);
	frame.set_visibility(true);
	while (frame.is_running()) {
		double deltatime = frame.handle_window();
		frame.clear_window();
		frame.display_performance();
		
		framebuffer.set_read_buffer(display_texture_index);
		display_texture_index = display_texture_index == 0 ? 1 : 0;

		framebuffer.blit_to_screen(0, 0, target_texture_0->get_size().x, target_texture_0->get_size().y, 0, 0, frame.window_width, frame.window_height,
			Framebuffer::Channel::COLOR, Framebuffer::Filter::LINEAR);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}