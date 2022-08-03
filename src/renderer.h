#pragma once

#include "core.h"

#include <SDL.h>
#undef main
#include <glad/glad.h>
#include <stb_image.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

namespace ssfw
{

namespace node_editor = ax::NodeEditor;

class renderer
{
private:
	struct window
	{
		SDL_Window *window;
		int width, height;
	};

	struct link
	{
		node_editor::LinkId self;
		node_editor::PinId in;
		node_editor::PinId out;
	};

	uint32_t ids = 1;
	uint32_t link_ids = 1;

public:
	int on_init();
	void on_destroy();
	void on_gui();
	void on_window_resize(int new_width, int new_height);

	void offset_cursor_by(int offset);

	void draw_generator_node();
	void draw_router_node();
	void draw_server_node();
	void draw_sink_node();
	void draw_pin(node_editor::PinKind kind, bool enabled);
	void draw_node_icon(const char *filepath);

	void clear();
	void new_frame();
	void update_viewport();
	void blit();
	void on_swap_window();

	SDL_GLContext glcontext;
	node_editor::EditorContext *node_editor_context = nullptr;
	window window_info;
	ImVector<link> links;
	bool first_frame = true;
};

} // namespace ssfw