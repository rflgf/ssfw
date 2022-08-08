#pragma once

#include "components/component.h"
#include "components/generator.h"
#include "components/router.h"
#include "components/server.h"
#include "components/sink.h"
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

class Renderer
{
private:
	struct Window
	{
		SDL_Window *window;
		int width, height;
	};

	struct Node
	{
		enum Type
		{
			GENERATOR,
			ROUTER,
			SERVER,
			SINK
		};

		struct Data
		{
			static constexpr uint8_t MAX_DATA_LENGTH = 10;

			std::string a;
			std::string b;
		};

		static constexpr uint8_t MAX_NAME_LENGTH = 50;

		std::string name;
		node_editor::PinId id;
		Type type;
		Data data;
		ImVec2 position;
		std::vector<node_editor::PinId> pins;
		std::vector<node_editor::LinkId> links;
	};

	struct Link
	{
		node_editor::LinkId self;
		node_editor::PinId in;
		node_editor::PinId out;
	};

	uint32_t ids = 0; // for nodes, pins and links.

public:
	int on_init();
	void on_destroy();
	void on_gui();
	void on_window_resize(int new_width, int new_height);

	void on_right_click_menu();
	void offset_cursor_by(int offset);

	void draw_nodes();
	void draw_generator_node(Node &g);
	void draw_router_node(Node &r);
	void draw_server_node(Node &s);
	void draw_sink_node(Node &s);
	void draw_pin(node_editor::PinKind kind, bool enabled);
	void draw_node_icon(const char *filepath);

	void clear();
	void new_frame();
	void update_viewport();
	void blit();
	void on_swap_window();

	SDL_GLContext gl_context;
	node_editor::EditorContext *node_editor_context = nullptr;
	Window window_info;
	std::vector<Link> links;
	std::vector<Node> nodes;
	bool first_frame = true;

	uint32_t generator_count = 0, router_count = 0, server_count = 0,
	         sink_count = 0;
};

} // namespace ssfw