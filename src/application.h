#pragma once

#include "core.h"

#include "SDL.h"
#undef main
#include "glad/glad.h"
#include "stb_image.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui_node_editor.h"

#include <imgui_internal.h>

namespace ssfw
{
namespace node_editor = ax::NodeEditor;

class application
{
public:
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

	int init();
	void deinit();
	void main_loop();
	// application() = default;
	// ~application() = default;
	// void render_topbar();
	// void show_statistics();

	void update_viewport();

private:
	SDL_GLContext glcontext;
	node_editor::EditorContext *node_editor_context = nullptr;
	window window_info;
	uint32_t ids = 1;

	void on_gui();
	void insert_generator_node();
};

} // namespace ssfw
