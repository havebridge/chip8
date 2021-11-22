#ifndef _WINDOW_H
#define _WINDOW_H

#include <SDL.h>
#include <stdio.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512


struct sdl_window
{
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;
	SDL_Texture* gTexture;
};

void window_init(struct sdl_window*);
void window_buffer(struct sdl_window*, uint32_t*, struct chip8*);
void window_update(struct sdl_window*, uint32_t*);
void window_close(struct sdl_window*);

#endif