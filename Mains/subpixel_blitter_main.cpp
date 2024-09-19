#include "GraphicsCortex.h"
#include "Interpolation/SubpixelBlitter.h"

int main() {

	Frame frame(0, 0, "ImageCortex", 0, 0, true, true, false, Frame::NOTIFICATION, true);
	Scene scene(frame);

	ImGuiIO& io = ImGui::GetIO(); // (void)io;
	ImFont* font = io.Fonts->AddFontFromFileTTF("Roboto-Thin.ttf", 12);

	std::shared_ptr<Image> image = std::make_shared<Image>("../ImageCortex/Images/0_3m_gsd_image.png", 3, true);
	
	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(*image,
		Texture2D::ColorTextureFormat::RGBA8, Texture2D::ColorFormat::RGB, Texture2D::Type::UNSIGNED_BYTE, 1, 0, 0);
	std::shared_ptr<Texture2D> target_texture_1 = std::make_shared<Texture2D>(texture->get_size().x / 2, texture->get_size().y / 2, Texture2D::ColorTextureFormat::RGBA8, 1, 0, 0);
	std::shared_ptr<Texture2D> target_texture_0 = std::make_shared<Texture2D>(texture->get_size().x / 2, texture->get_size().y / 2, Texture2D::ColorTextureFormat::RGBA8, 1, 0, 0);

	std::shared_ptr<Texture2D> merge_texture = std::make_shared<Texture2D>(target_texture_0->get_size().x *2 , target_texture_0->get_size().y * 2, Texture2D::ColorTextureFormat::RGBA32F, 1, 0, 0);


	SubpixelBlitter blitter;
	TextureArithmatic arithmatic;

	Framebuffer framebuffer;
	framebuffer.attach_color(0, target_texture_0);
	framebuffer.attach_color(1, target_texture_1);
	framebuffer.attach_color(2, merge_texture);
	framebuffer.attach_color(3, texture);
	
	int display_texture_index = 1;
	float offset_a = -0.5;
	float offset_b = 0;
	bool once = true;
	frame.resize(target_texture_0->get_size().x, target_texture_0->get_size().y);
	frame.set_visibility(true);
	while (frame.is_running()) {

		double deltatime = frame.handle_window();
		frame.clear_window();
		frame.display_performance();


		ImGui::PushFont(font);
		ImGui::Begin("##Quincunx");

		ImGui::BeginChild("##QuincunxPanel");
		ImGui::SliderFloat("offset_a", &offset_a, -10, 10);
		ImGui::SliderFloat("offset_b", &offset_b, -10, 10);
		ImGui::EndChild();
		ImGui::End();
		
		blitter.blit(*texture, *target_texture_0, glm::vec2(0.0, 0.0), glm::vec2(texture->get_size()) + glm::vec2(0.0, 0.0), true);
		blitter.blit(*texture, *target_texture_1, glm::vec2(offset_a, offset_a), glm::vec2(texture->get_size()) + glm::vec2(offset_a, offset_a), true);

		arithmatic.operation_unary(*merge_texture, "vec4(0)");

		blitter.merge(
			std::vector{ std::ref(*target_texture_0), std::ref(*target_texture_1) },
			*merge_texture,
			{ glm::vec2(0.0, 0.0), glm::vec2(offset_b, offset_b) },
			{ glm::vec2(merge_texture->get_size() / 2) + glm::vec2(0.0, 0.0), glm::vec2(merge_texture->get_size() / 2) + glm::vec2(offset_b, offset_b) },
			true
		);


		if (frame.get_key_press(Frame::A))			display_texture_index = 0;
		else if (frame.get_key_press(Frame::B))		display_texture_index = 1;
		else if (frame.get_key_press(Frame::SPACE)) display_texture_index = 2;
		else if (frame.get_key_press(Frame::O))		display_texture_index = 3;
		framebuffer.set_read_buffer(display_texture_index);

		glm::ivec2 resolution = display_texture_index >= 2 ? merge_texture->get_size() : target_texture_0->get_size();

		framebuffer.blit_to_screen(0, 0, resolution.x, resolution.y, 0, 0, frame.window_width, frame.window_height,
			Framebuffer::Channel::COLOR, Framebuffer::Filter::LINEAR);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		ImGui::PopFont();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}