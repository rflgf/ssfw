#include "application.h"
#include "utils.h"

#include <imgui_impl_sdl.h>

namespace ssfw
{

namespace node_editor = ax::NodeEditor;

void Application::handle_events()
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
					renderer.on_window_resize(e.window.data1, e.window.data2);
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
						running = false;
					break;
				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
	}
}

void Application::main_loop()
{
	while (running)
	{
		handle_events();
		renderer.clear();
		renderer.new_frame();
		bool should_simulate = renderer.on_gui();
		if (should_simulate)
		{
			// and this is the point where I realize Renderer is responsible
			// for way more tasks than it should be...
			// FIXME(Rafael): bring all logic to the Application abstraction and
			// have Renderer only be responsible for rendering issued UI
			// elements.
			// model.load_from(renderer.model_filepath);
			// renderer.model_view = model;
		}
		renderer.blit();
		renderer.on_swap_window();
	}
}

void Application::run()
{
	renderer.on_init();
	main_loop();
	renderer.on_destroy();
}

} // namespace ssfw