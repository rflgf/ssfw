#include "renderer.h"

#include "utils.h"

namespace ssfw
{

int renderer::on_init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
	                    SDL_GL_CONTEXT_PROFILE_CORE);

	window_info.width = 1280;
	window_info.height = 720;

	window_info.window = SDL_CreateWindow(
	    "Simple Simulation Framework", SDL_WINDOWPOS_UNDEFINED,
	    SDL_WINDOWPOS_UNDEFINED, window_info.width, window_info.height,
	    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (window_info.window == nullptr)
	{
		SDL_Log("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	glcontext = SDL_GL_CreateContext(window_info.window);

	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		SDL_Log("Failed to initialize OpenGL context");
		return -1;
	}

	const auto *renderer = glGetString(GL_RENDERER); // get renderer string
	const auto *version = glGetString(GL_VERSION);   // version as a string
	SDL_Log("Renderer: %s\n", renderer);
	SDL_Log("Version: %s\n", version);

	// imgui setup
	const char *glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.WantCaptureMouse = true;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window_info.window, glcontext);
	ImGui_ImplOpenGL3_Init(glsl_version);

	update_viewport();

	node_editor::Config config;
	config.SettingsFile = "textgen-nodes.json";
	node_editor_context = node_editor::CreateEditor(&config);
}

void renderer::on_destroy()
{
	node_editor::DestroyEditor(node_editor_context);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window_info.window);

	SDL_Quit();
}

void renderer::clear()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void renderer::new_frame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window_info.window);
	ImGui::NewFrame();
}

void renderer::update_viewport()
{
	glViewport(0, 0, window_info.width, window_info.height);
};

void renderer::on_gui()
{
	auto &io = ImGui::GetIO();
	ImGui::SetNextWindowPos({0, 0});
	ImGui::SetNextWindowSize(io.DisplaySize);

	const auto flags = ImGuiWindowFlags_NoTitleBar;

	ImGui::Begin("window", nullptr, flags);
	// ImGui::Begin("window");

	node_editor::SetCurrentEditor(node_editor_context);
	node_editor::Begin("My Editor", ImVec2(0.0, 0.0f));

	renderer::ids = 1;

	draw_generator_node();
	draw_router_node();
	draw_server_node();
	draw_sink_node();
	draw_sink_node();

	for (auto &linkInfo : links)
		node_editor::Link(linkInfo.self, linkInfo.in, linkInfo.out);

	// for (auto &linkInfo : links)
	// 	std::cout << linkInfo.self.Get() << ' ' << linkInfo.in.Get() << ' '
	// 	          << linkInfo.out.Get() << '\n';

	if (node_editor::BeginCreate())
	{
		node_editor::PinId inputPinId, outputPinId;
		if (node_editor::QueryNewLink(&inputPinId, &outputPinId))
		{
			if (inputPinId && outputPinId) // both are valid, let's accept link
			{
				// node_editor::AcceptNewItem() return true when user release
				// mouse button.
				if (node_editor::AcceptNewItem())
				{
					// Since we accepted new link, lets add one to our list of
					// links.
					links.push_back(
					    {node_editor::LinkId(renderer::link_ids++), inputPinId,
					     outputPinId});

					// Draw new link.
					node_editor::Link(links.back().self, links.back().in,
					                  links.back().out);
				}

				// You may choose to reject connection between these nodes
				// by calling node_editor::RejectNewItem(). This will allow
				// editor to give visual feedback by changing link thickness and
				// color.
			}
		}
	}
	node_editor::EndCreate(); // Wraps up object creation action handling.

	node_editor::End();
	node_editor::SetCurrentEditor(nullptr);

	ImGui::End();
}

void renderer::draw_node_icon(const char *filepath)
{
	int width = 0;
	int height = 0;
	GLuint texture = 0;
	float resize = 0.3f;
	bool load_ok =
	    utils::load_texture_from_file(filepath, &texture, &width, &height);
	SSFW_ASSERT(load_ok, "Failed to load texture resource `%s`.", filepath);
	ImGui::Image((void *)(intptr_t)texture, ImVec2(static_cast<float>(width), static_cast<float>(height)) * resize);
}

void renderer::draw_pin(node_editor::PinKind kind, bool enabled)
{
	auto color =
	    enabled ? IM_COL32(73, 190, 37, 200) : IM_COL32(190, 77, 37, 130);
	ImVec2 cur_pos = ImGui::GetCursorPos();
	float rad = 5.0f;
	float padding = 1.0f;
	ImVec2 window_center {cur_pos.x + rad + padding, cur_pos.y + rad + padding};
	ImGui::GetWindowDrawList()->AddCircle(window_center, rad, color, 10, 2);
	// ImGui::GetWindowDrawList()->AddCircle(window_center, rad, color, 0, 2);
	ImGui::SetCursorPos({cur_pos.x + rad * 2 + padding, cur_pos.y});
}

void renderer::offset_cursor_by(int offset)
{
	ImGui::SetCursorPos(
	    {ImGui::GetCursorPosX() + offset, ImGui::GetCursorPosY()});
}

void renderer::draw_generator_node()
{
	int lower_throughput, upper_throughput;
	auto id = renderer::ids++;
	node_editor::BeginNode(id);
	draw_node_icon("assets/generator.png");
	ImGui::Text("Generator");
	ImGui::Text("");
	static char str1[4];
	static char str2[4];

	{
		ImGui::Text("throughput:");
		ImGui::Text("one entity every");
		ImGui::PushItemWidth(40);
		ImGui::InputText("", str1, IM_ARRAYSIZE(str1));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("to");
		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		ImGui::InputText("", str2, IM_ARRAYSIZE(str2));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("t.u.");
		ImGui::Text("");
	}

	{
		offset_cursor_by(124);
		node_editor::BeginPin(renderer::ids++, node_editor::PinKind::Output);
		ImGui::Text("out");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
	}

	node_editor::EndNode();
}

void renderer::draw_router_node()
{
	auto id = renderer::ids++;
	node_editor::BeginNode(id);
	draw_node_icon("assets/router.png");
	ImGui::Text("Router");
	ImGui::Text("");

	{
		node_editor::BeginPin(renderer::ids++, node_editor::PinKind::Input);
		draw_pin(node_editor::PinKind::Output, false);
		ImGui::Text(" in");
		node_editor::EndPin();
	}

	{
		ImGui::SameLine(60);
		node_editor::BeginPin(renderer::ids++, node_editor::PinKind::Output);
		ImGui::Text("out a");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
	}

	{
		ImGui::Text(" ");
		ImGui::SameLine(60);
		node_editor::BeginPin(renderer::ids++, node_editor::PinKind::Output);
		ImGui::Text("out b");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
	}

	node_editor::EndNode();
}

void renderer::draw_server_node()
{
	int attendants, lower_service_time, upper_service_time;
	auto id = renderer::ids++;
	node_editor::BeginNode(id);
	draw_node_icon("assets/server.png");
	ImGui::Text("Server");
	ImGui::Text("");
	static char str1[4];
	static char str2[4];

	{
		ImGui::Text("service time:");
		ImGui::PushItemWidth(40);
		ImGui::InputText("", str1, IM_ARRAYSIZE(str1));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("to");
		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		ImGui::InputText("", str2, IM_ARRAYSIZE(str2));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("t.u.");
		ImGui::Text("per entity.");
		ImGui::Text("");
	}

	{
		node_editor::BeginPin(renderer::ids++, node_editor::PinKind::Input);
		draw_pin(node_editor::PinKind::Output, false);
		ImGui::Text(" in");
		node_editor::EndPin();
	}

	{
		ImGui::SameLine(120);
		node_editor::BeginPin(renderer::ids++, node_editor::PinKind::Output);
		ImGui::Text("out");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
	}

	node_editor::EndNode();
}

void renderer::draw_sink_node()
{
	auto id = renderer::ids++;
	node_editor::BeginNode(id);
	draw_node_icon("assets/sink.png");
	ImGui::Text("Sink");
	ImGui::Text("");

	{
		node_editor::BeginPin(renderer::ids++, node_editor::PinKind::Input);
		draw_pin(node_editor::PinKind::Output, false);
		ImGui::Text(" in");
		node_editor::EndPin();
	}

	node_editor::EndNode();
}

void renderer::on_window_resize(int new_width, int new_height)
{
	window_info.width = new_width;
	window_info.height = new_height;
	update_viewport();
}

void renderer::on_swap_window() { SDL_GL_SwapWindow(window_info.window); }

void renderer::blit()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
} // namespace ssfw