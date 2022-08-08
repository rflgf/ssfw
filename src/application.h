#pragma once

#include "core.h"
#include "model.h"
#include "renderer.h"

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

class Application
{
private:
	bool running = true;
	Renderer renderer;
	Model model;

public:
	void run();
	void main_loop();
	void handle_events();
};

} // namespace ssfw
