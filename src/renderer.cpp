#include "renderer.h"

#include <format>
#include <sstream>

#include "utils.h"

#include <imgui_stdlib.h>
#include <nfd.h>

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
	config.SettingsFile = "nodes.json";
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

void Renderer::draw_main_menu()
{
	static bool error_modal = false;
	auto const file_dialog = [](std::string &destination)
	{
		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);
		if (result == NFD_OKAY)
		{
			destination = std::string(outPath);
			free(outPath);
		}
		else if (result != NFD_CANCEL)
			std::cout << ("Error: %s\n", NFD_GetError()) << '\n';
	};

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open...", NULL))
			{
				// PS: no change tracker implemented! all changes are lost!
				file_dialog(model_filepath);
				load_model_from_file(model_filepath);
			}

			else if (ImGui::MenuItem("Save", NULL))
			{
				if (model_filepath.empty())
					file_dialog(model_filepath);
				save_model_to_file(model_filepath);
			}

			else if (ImGui::MenuItem("Save as...", NULL))
			{
				// PS: no change tracker implemented! all changes are lost!
				file_dialog(model_filepath);
				save_model_to_file(model_filepath);
			}

			ImGui::EndMenu();
		}

		if (current_view == View::MODELER)
			if (ImGui::BeginMenu("Simulate"))
			{
				current_view = View::SIMULATOR;

				model_view = Renderer::renderer_rep_to_model_rep(nodes);

				error_modal = !model_view.validate();
				if (error_modal)
				{
					current_view = View::MODELER;
					model_validation_errors.emplace_back("some error D:");
					ImGui::EndMenu();
					ImGui::EndMainMenuBar();
					goto modal_check;
				}

				ImGui::EndMenu();
			}
		ImGui::EndMainMenuBar();
	}
modal_check:
	if (error_modal)
		draw_centered_modal(model_validation_errors, error_modal);
}

bool Renderer::on_gui()
{
	draw_main_menu();

	ImGuiIO &io = ImGui::GetIO();
	node_editor::SetCurrentEditor(node_editor_context);

	const auto FLAGS = ImGuiWindowFlags_NoTitleBar;
	const int MENU_BAR_HEIGHT = 19;
	ImGui::SetNextWindowPos({0, MENU_BAR_HEIGHT});
	ImGui::SetNextWindowSize(
	    {io.DisplaySize.x, io.DisplaySize.y - MENU_BAR_HEIGHT});
	ImGui::Begin("window", nullptr, FLAGS);
	node_editor::Begin("My Editor", {0, 0});

	on_right_click_menu();
	draw_nodes();

	for (Link &linkInfo : links)
		node_editor::Link(linkInfo.id, linkInfo.in, linkInfo.out);

	if (node_editor::BeginCreate())
	{
		node_editor::PinId inputPinId, outputPinId;
		if (node_editor::QueryNewLink(&inputPinId, &outputPinId))
		{
			if (pins.at(inputPinId.Get()).parent.id ==
			    pins.at(outputPinId.Get()).parent.id)
				node_editor::RejectNewItem();

			if (pins.at(inputPinId.Get()).type ==
			    pins.at(outputPinId.Get()).type)
				node_editor::RejectNewItem();

			if (inputPinId && outputPinId)
			{
				if (pins.at(inputPinId.Get()).type ==
				    node_editor::PinKind::Output)
				{
					node_editor::PinId temp = inputPinId;
					inputPinId = outputPinId;
					outputPinId = temp;
				}

				if (node_editor::AcceptNewItem())
				{
					links.push_back(
					    {node_editor::LinkId(ids++), inputPinId, outputPinId});

					node_editor::Link(links.back().id, links.back().in,
					                  links.back().out);
				}
			}
		}
	}
	node_editor::EndCreate();

	if (node_editor::BeginDelete())
	{
		node_editor::LinkId deleted_link_id;
		while (node_editor::QueryDeletedLink(&deleted_link_id))
			if (node_editor::AcceptDeletedItem())
				for (auto link = links.begin(); link != links.end(); link++)
					if ((*link).id == deleted_link_id)
					{
						links.erase(link);
						break;
					}
	}
	node_editor::EndDelete();

	node_editor::End();
	node_editor::SetCurrentEditor(nullptr);

	ImGui::End();

	return false;
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
	uint32_t color =
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
		ImGui::Text("out");
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
		Node n;
		n.position = ImGui::GetMousePos();
		n.id = ids++;

		if (ImGui::MenuItem("Add generator node"))
		{
			n.type = Type::GENERATOR;
			n.name = std::format("Generator {}", ++generator_count);

			node_editor::PinId p1_id = n.pins.emplace_back(ids++); // out a
			pins.emplace(std::piecewise_construct,
			             std::forward_as_tuple(p1_id.Get()),
			             std::forward_as_tuple(
			                 p1_id.Get(), node_editor::PinKind::Output, n));

			nodes.push_back(n);
		}
		else if (ImGui::MenuItem("Add router node"))
		{
			n.type = Type::ROUTER;
			n.name = std::format("Router {}", ++router_count);

			node_editor::PinId p1_id = n.pins.emplace_back(ids++); // in
			pins.emplace(std::piecewise_construct,
			             std::forward_as_tuple(p1_id.Get()),
			             std::forward_as_tuple(p1_id.Get(),
			                                   node_editor::PinKind::Input, n));
			node_editor::PinId p2_id = n.pins.emplace_back(ids++); // out a
			pins.emplace(std::piecewise_construct,
			             std::forward_as_tuple(p2_id.Get()),
			             std::forward_as_tuple(
			                 p2_id.Get(), node_editor::PinKind::Output, n));
			node_editor::PinId p3_id = n.pins.emplace_back(ids++); // out b
			pins.emplace(std::piecewise_construct,
			             std::forward_as_tuple(p3_id.Get()),
			             std::forward_as_tuple(
			                 p3_id.Get(), node_editor::PinKind::Output, n));

			nodes.push_back(n);
		}
		else if (ImGui::MenuItem("Add server node"))
		{
			n.type = Type::SERVER;
			n.name = std::format("Server {}", ++server_count);

			node_editor::PinId p1_id = n.pins.emplace_back(ids++); // in
			pins.emplace(std::piecewise_construct,
			             std::forward_as_tuple(p1_id.Get()),
			             std::forward_as_tuple(p1_id.Get(),
			                                   node_editor::PinKind::Input, n));
			node_editor::PinId p2_id = n.pins.emplace_back(ids++); // out a
			pins.emplace(std::piecewise_construct,
			             std::forward_as_tuple(p2_id.Get()),
			             std::forward_as_tuple(
			                 p2_id.Get(), node_editor::PinKind::Output, n));

			nodes.push_back(n);
		}
		else if (ImGui::MenuItem("Add sink node"))
		{
			n.type = Type::SINK;
			n.name = std::format("Sink {}", ++sink_count);

			node_editor::PinId p1_id = n.pins.emplace_back(ids++); // in
			pins.emplace(std::piecewise_construct,
			             std::forward_as_tuple(p1_id.Get()),
			             std::forward_as_tuple(p1_id.Get(),
			                                   node_editor::PinKind::Input, n));

			nodes.push_back(n);
		}
		ImGui::EndPopup();
	}
}

void Renderer::draw_nodes()
{
	for (Node &node : nodes)
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

void Renderer::load_model_from_file(std::string_view model_filepath)
{
	// shouldnt this be in application?
}

void Renderer::draw_centered_modal(std::vector<std::string> &messages,
                                   bool &modal_open)
{
	ImGui::OpenPopup("Errors");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Errors", NULL,
	                           ImGuiWindowFlags_AlwaysAutoResize))
	{
		for (const std::string &m : messages)
			ImGui::Text(m.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			modal_open = false;
			ImGui::CloseCurrentPopup();
			model_validation_errors.clear();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void Renderer::save_model_to_file(std::string_view filepath) {}

Model Renderer::renderer_rep_to_model_rep(std::vector<Node> nodes)
{
	Model m;

	// std::unordered_map<uintptr_t, Node &> nodes_map;
	// for (Node &n : nodes)
	// 	nodes_map.emplace(n.id.Get(), n);

	// for (const Node &n : nodes)
	// {
	// 	switch (n.type)
	// 	{
	// 	case Node::Type::GENERATOR:
	// 		entity lt = static_cast<entity>(std::atoi(n.data.a.c_str()));
	// 		entity ut = static_cast<entity>(std::atoi(n.data.b.c_str()));
	// 		m.generator_components.emplace_back(
	// 		    n.name, static_cast<uint32_t>(n.id.Get()), nullptr, lt, ut);
	// 	}
	// }

	return m;
}

} // namespace ssfw