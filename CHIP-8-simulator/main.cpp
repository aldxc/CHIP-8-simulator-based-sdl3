#include<iostream>
#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>

int SDL_main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVENTS);
	SDL_Window* window = SDL_CreateWindow("a", 1080, 960, 0);

	bool running = true;

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (SDL_EVENT_QUIT == event.type) {
				running = false;
				break;
			}
		}
	}

	SDL_DestroyWindow(window);
	
	return 0;
}