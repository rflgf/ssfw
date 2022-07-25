#include "application.h"
#include "utils.h"
#include "imgui_impl_sdl.h"

namespace ssfw
{

int application::init()
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
	    "textgen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	    window_info.width, window_info.height,
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
	io.WantCaptureMouse = true;

	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable |
	// ImGuiWindowFlags_MenuBar;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(window_info.window, glcontext);
	ImGui_ImplOpenGL3_Init(glsl_version);

	update_viewport();

	node_editor::Config config;
	config.SettingsFile = "textgen-nodes.json";
	node_editor_context = node_editor::CreateEditor(&config);
}

void application::deinit()
{

	node_editor::DestroyEditor(node_editor_context);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window_info.window);

	SDL_Quit();
}

void application::main_loop()
{
	bool running = true;
	while (running)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (!ImGui_ImplSDL2_ProcessEvent(&e))
			{
				switch (e.type)
				{
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					{
						window_info.width = e.window.data1;
						window_info.height = e.window.data2;
						update_viewport();
					}
					break;
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
				{
					const bool down = e.type == SDL_KEYDOWN;

					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						if (!down)
						{
							running = false;
						}
						break;
					default:
						// ignore other keys
						break;
					}
				}
				break;
				default:
					// ignore other events
					break;
				}
			}
		}

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		{
			// ImGui:: SetNextWindowSize({io.DisplaySize.x, io.DisplaySize.y});
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(window_info.window);
			ImGui::NewFrame();

			on_gui();

			ImGui::Render();

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		SDL_GL_SwapWindow(window_info.window);
	}
}

void application::on_gui()
{
	auto &io = ImGui::GetIO();
	ImGui::SetNextWindowPos({0, 0});
	ImGui::SetNextWindowSize(io.DisplaySize);

	const auto flags = ImGuiWindowFlags_NoTitleBar;

	// ImGui::Begin("window", nullptr, flags);
	ImGui::Begin("window");

	node_editor::SetCurrentEditor(node_editor_context);
	node_editor::Begin("My Editor", ImVec2(0.0, 0.0f));

	ids = 1;

	add_generator_node();
	add_generator_node();

	for (auto& linkInfo : m_Links)
		node_editor::Link(linkInfo.self, linkInfo.in, linkInfo.out);

	if (node_editor::BeginCreate())
        {
            node_editor::PinId inputPinId, outputPinId;
            if (node_editor::QueryNewLink(&inputPinId, &outputPinId))
            {
                // QueryNewLink returns true if editor want to create new link between pins.
                //
                // Link can be created only for two valid pins, it is up to you to
                // validate if connection make sense. Editor is happy to make any.
                //
                // Link always goes from input to output. User may choose to drag
                // link from output pin or input pin. This determine which pin ids
                // are valid and which are not:
                //   * input valid, output invalid - user started to drag new ling from input pin
                //   * input invalid, output valid - user started to drag new ling from output pin
                //   * input valid, output valid   - user dragged link over other pin, can be validated

                if (inputPinId && outputPinId) // both are valid, let's accept link
                {
                    // node_editor::AcceptNewItem() return true when user release mouse button.
                    if (node_editor::AcceptNewItem())
                    {
                        // Since we accepted new link, lets add one to our list of links.
                        m_Links.push_back({ node_editor::LinkId(ids++), inputPinId, outputPinId });

                        // Draw new link.
                        node_editor::Link(m_Links.back().self, m_Links.back().in, m_Links.back().out);
                    }

                    // You may choose to reject connection between these nodes
                    // by calling node_editor::RejectNewItem(). This will allow editor to give
                    // visual feedback by changing link thickness and color.
                }
            }
        }
        node_editor::EndCreate(); // Wraps up object creation action handling.


	node_editor::End();
	node_editor::SetCurrentEditor(nullptr);

	ImGui::End();
}

void draw_node_icon(const char *filepath)
{
	int my_image_width = 0;
	int my_image_height = 0;
	GLuint my_image_texture = 0;
	double factor = 0.2;
	bool ret = utils::load_texture_from_file(filepath, &my_image_texture,
	                                         &my_image_width, &my_image_height);
	// IM_ASSERT(ret);
	ImGui::Image(
	    (void *)(intptr_t)my_image_texture,
	    ImVec2(my_image_width * node_editor::GetCurrentZoom() * factor,
	           my_image_height * node_editor::GetCurrentZoom() * factor));
}

void application::add_generator_node()
{
	int lower_throughput, upper_throughput;
	auto id = ids++;
	node_editor::BeginNode(id);
	draw_node_icon("assets/generator.png");
	ImGui::Text("Generator");
	// ImGui::InputText();
	static char str1[128] = "";
	static char str2[128] = "";
	ImGui::Text("lower throughput");
	ImGui::PushItemWidth(25);
	ImGui::SameLine();
	ImGui::InputText("", str1, IM_ARRAYSIZE(str1));
	ImGui::Text("upper throughput");
	ImGui::SameLine();
	ImGui::InputText("", str2, IM_ARRAYSIZE(str2));
	ImGui::PopItemWidth();

	node_editor::BeginPin(ids++, node_editor::PinKind::Output);
	ImGui::Text("out");
	node_editor::EndPin();
	node_editor::EndNode();
}

void application::add_router_node()
{
	auto id = ids++;
	node_editor::BeginNode(id);
	draw_node_icon("assets/router.png");
	ImGui::Text("Router");
	node_editor::BeginPin(ids++, node_editor::PinKind::Output);
	ImGui::Text("out");
	node_editor::EndPin();
	node_editor::EndNode();
}

void application::update_viewport()
{
	glViewport(0, 0, window_info.width, window_info.height);
};

} // namespace ssfw