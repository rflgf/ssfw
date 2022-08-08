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
		renderer.on_gui();
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