#include "renderer.h"

#include <format>
#include <sstream>

#include "utils.h"

#include "imgui_stdlib.h"

namespace ssfw
{

int Renderer::on_init()
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

	gl_context = SDL_GL_CreateContext(window_info.window);

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

	ImGui_ImplSDL2_InitForOpenGL(window_info.window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	update_viewport();

	node_editor::Config config;
	config.SettingsFile = "textgen-nodes.json";
	node_editor_context = node_editor::CreateEditor(&config);
}

void Renderer::on_destroy()
{
	node_editor::DestroyEditor(node_editor_context);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window_info.window);

	SDL_Quit();
}

void Renderer::clear()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::new_frame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window_info.window);
	ImGui::NewFrame();
}

void Renderer::update_viewport()
{
	glViewport(0, 0, window_info.width, window_info.height);
};

void Renderer::on_gui()
{
	auto &io = ImGui::GetIO();
	ImGui::SetNextWindowPos({0, 0});
	ImGui::SetNextWindowSize(io.DisplaySize);

	const auto flags = ImGuiWindowFlags_NoTitleBar;

	ImGui::Begin("window", nullptr, flags);
	// ImGui::ShowStackToolWindow();

	node_editor::SetCurrentEditor(node_editor_context);
	node_editor::Begin("My Editor", ImVec2(0.0, 0.0f));

	on_right_click_menu();

	draw_nodes();

	for (auto &linkInfo : links)
		node_editor::Link(linkInfo.self, linkInfo.in, linkInfo.out);

	if (node_editor::BeginCreate())
	{
		node_editor::PinId inputPinId, outputPinId;
		if (node_editor::QueryNewLink(&inputPinId, &outputPinId))
		{
			if (inputPinId && outputPinId)
			{
				if (node_editor::AcceptNewItem())
				{
					links.push_back(
					    {node_editor::LinkId(ids++), inputPinId, outputPinId});

					node_editor::Link(links.back().self, links.back().in,
					                  links.back().out);
				}
			}
			// node_editor::RejectNewItem();
		}
	}
	node_editor::EndCreate();

	if (node_editor::BeginDelete())
	{
		node_editor::LinkId deleted_link_id;
		while (node_editor::QueryDeletedLink(&deleted_link_id))
			if (node_editor::AcceptDeletedItem())
				for (auto link = links.begin(); link != links.end(); link++)
					if ((*link).self == deleted_link_id)
					{
						links.erase(link);
						break;
					}
	}
	node_editor::EndDelete();

	node_editor::End();
	node_editor::SetCurrentEditor(nullptr);

	ImGui::End();
}

void Renderer::draw_node_icon(const char *filepath)
{
	int width = 0;
	int height = 0;
	GLuint texture = 0;
	float resize = 0.3f;
	bool load_ok =
	    utils::load_texture_from_file(filepath, &texture, &width, &height);
	SSFW_ASSERT(load_ok, "Failed to load texture resource `%s`.", filepath);
	ImGui::Image((void *)(intptr_t)texture,
	             ImVec2(static_cast<float>(width), static_cast<float>(height)) *
	                 resize);
}

void Renderer::draw_pin(node_editor::PinKind kind, bool enabled)
{
	auto color =
	    enabled ? IM_COL32(73, 190, 37, 200) : IM_COL32(190, 77, 37, 130);
	ImVec2 cur_pos = ImGui::GetCursorPos();
	float rad = 5.0f;
	float padding = 1.0f;
	ImVec2 window_center {cur_pos.x + rad + padding, cur_pos.y + rad + padding};
	ImGui::GetWindowDrawList()->AddCircle(window_center, rad, color, 10, 2);
	ImGui::SetCursorPos({cur_pos.x + rad * 2 + padding, cur_pos.y});
}

void Renderer::offset_cursor_by(int offset)
{
	ImGui::SetCursorPos(
	    {ImGui::GetCursorPosX() + offset, ImGui::GetCursorPosY()});
}

void Renderer::draw_generator_node(Node &g)
{
	node_editor::BeginNode(g.id.Get());
	draw_node_icon("assets/generator.png");
	ImGui::Text("Generator");
	ImGui::Text("name:");
	ImGui::SameLine();
	ImGui::PushItemWidth(80);
	ImGui::InputText(std::format("##1{}", g.id.Get()).c_str(), &g.name);
	ImGui::PopItemWidth();
	ImGui::Text("");
	{
		ImGui::Text("throughput:");
		ImGui::Text("one entity every");
		ImGui::PushItemWidth(40);
		ImGui::InputText(std::format("##2{}", g.id.Get()).c_str(), &g.data.a);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("to");
		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		ImGui::InputText(std::format("##3{}", g.id.Get()).c_str(), &g.data.b);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("t.u.");
		ImGui::Text("");
	}

	{
		offset_cursor_by(80);
		ImGui::PushID(g.pins.at(0).AsPointer());
		node_editor::BeginPin(g.pins.at(0), node_editor::PinKind::Output);
		ImGui::Text("outttttt");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
		ImGui::PopID();
	}

	node_editor::EndNode();
}

void Renderer::draw_router_node(Node &r)
{
	node_editor::BeginNode(r.id.Get());
	draw_node_icon("assets/router.png");
	ImGui::Text("Router");
	ImGui::Text("name:");
	ImGui::SameLine(0);
	ImGui::PushItemWidth(100);
	ImGui::InputText(std::format("##1{}", r.id.Get()).c_str(), &r.name);
	ImGui::PopItemWidth();
	ImGui::Text("");

	{
		node_editor::BeginPin(r.pins.at(0), node_editor::PinKind::Input);
		draw_pin(node_editor::PinKind::Output, false);
		ImGui::Text("in");
		node_editor::EndPin();
	}

	{
		ImGui::SameLine(60);
		node_editor::BeginPin(r.pins.at(1), node_editor::PinKind::Output);
		ImGui::Text("out a");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
	}

	{
		ImGui::Text(" ");
		ImGui::SameLine(60);
		node_editor::BeginPin(r.pins.at(2), node_editor::PinKind::Output);
		ImGui::Text("out b");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
	}

	node_editor::EndNode();
}

void Renderer::draw_server_node(Node &s)
{
	node_editor::BeginNode(s.id.Get());
	draw_node_icon("assets/server.png");
	ImGui::Text("Server");
	ImGui::Text("name:");
	ImGui::SameLine();
	ImGui::PushItemWidth(100);
	ImGui::InputText(std::format("##1{}", s.id.Get()).c_str(), &s.name);
	ImGui::PopItemWidth();
	ImGui::Text("");

	{
		ImGui::Text("service time:");
		ImGui::PushItemWidth(40);
		ImGui::InputText(std::format("##2{}", s.id.Get()).c_str(), &s.data.a);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("to");
		ImGui::SameLine();
		ImGui::PushItemWidth(40);
		ImGui::InputText(std::format("##3{}", s.id.Get()).c_str(), &s.data.b);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("t.u.");
		ImGui::Text("per entity.");
		ImGui::Text("");
	}

	{
		node_editor::BeginPin(s.pins.at(0), node_editor::PinKind::Input);
		draw_pin(node_editor::PinKind::Output, false);
		ImGui::Text(" in");
		node_editor::EndPin();
	}

	{
		ImGui::SameLine(120);
		node_editor::BeginPin(s.pins.at(1), node_editor::PinKind::Output);
		ImGui::Text("out");
		ImGui::SameLine(0);
		draw_pin(node_editor::PinKind::Output, false);
		node_editor::EndPin();
	}

	node_editor::EndNode();
}

void Renderer::draw_sink_node(Node &s)
{
	node_editor::BeginNode(s.id.Get());
	draw_node_icon("assets/sink.png");
	ImGui::Text("Sink");
	ImGui::Text("name:");
	ImGui::SameLine();
	ImGui::PushItemWidth(100);
	ImGui::InputText(std::format("##1{}", s.id.Get()).c_str(), &s.name);
	ImGui::PopItemWidth();
	ImGui::Text("");

	{
		node_editor::BeginPin(s.pins.at(0), node_editor::PinKind::Input);
		draw_pin(node_editor::PinKind::Output, false);
		ImGui::Text(" in");
		node_editor::EndPin();
	}

	node_editor::EndNode();
}

void Renderer::on_window_resize(int new_width, int new_height)
{
	window_info.width = new_width;
	window_info.height = new_height;
	update_viewport();
}

void Renderer::on_swap_window() { SDL_GL_SwapWindow(window_info.window); }

void Renderer::blit()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::on_right_click_menu()
{
	if (ImGui::BeginPopupContextWindow())
	{
		using Type = Node::Type;

		if (ImGui::MenuItem("Add generator node"))
		{
			auto &n = nodes.emplace_back();
			n.position = ImGui::GetMousePos();
			n.type = Type::GENERATOR;
			n.id = ids++;
			n.name = std::format("Generator {}", ++generator_count);
			n.pins.emplace_back(ids++); // out
		}
		else if (ImGui::MenuItem("Add router node"))
		{
			auto &n = nodes.emplace_back();
			n.position = ImGui::GetMousePos();
			n.type = Type::ROUTER;
			n.id = ids++;
			n.name = std::format("Router {}", ++router_count);
			n.pins.emplace_back(ids++); // in
			n.pins.emplace_back(ids++); // out a
			n.pins.emplace_back(ids++); // out b
		}
		else if (ImGui::MenuItem("Add server node"))
		{
			auto &n = nodes.emplace_back();
			n.position = ImGui::GetMousePos();
			n.type = Type::SERVER;
			n.id = ids++;
			n.name = std::format("Server {}", ++server_count);
			n.pins.emplace_back(ids++); // in
			n.pins.emplace_back(ids++); // out
		}
		else if (ImGui::MenuItem("Add sink node"))
		{
			auto &n = nodes.emplace_back();
			n.position = ImGui::GetMousePos();
			n.type = Type::SINK;
			n.id = ids++;
			n.name = std::format("Sink {}", ++sink_count);
			n.pins.emplace_back(ids++); // in
		}
		ImGui::EndPopup();
	}
}

void Renderer::draw_nodes()
{
	for (auto &node : nodes)
		switch (node.type)
		{
		case Node::Type::GENERATOR:
			draw_generator_node(node);
			break;

		case Node::Type::ROUTER:
			draw_router_node(node);
			break;

		case Node::Type::SERVER:
			draw_server_node(node);
			break;

		case Node::Type::SINK:
			draw_sink_node(node);
			break;
		default:
			SSFW_ASSERT(0, "Bad node type.");
		}
}
} // namespace ssfw