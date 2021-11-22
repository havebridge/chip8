#include "window.h"
#include "chip8.h"

void window_init(struct sdl_window* win)
{
	win->gWindow = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!win->gWindow)
	{
		printf("The window init error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	win->gRenderer = SDL_CreateRenderer(win->gWindow, -1, 0);
	if (!win->gRenderer)
	{
		printf("The renderer init error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	win->gTexture = SDL_CreateTexture(win->gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
	if (!win->gTexture)
	{
		printf("The texture init error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_GetRenderDrawColor(win->gRenderer, 0, 0, 0, 0);
	SDL_RenderClear(win->gRenderer);
	SDL_RenderPresent(win->gRenderer);
}

void window_buffer(struct sdl_window* win, uint32_t* buffer, struct chip8* chip)
{
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			uint8_t pixel = chip->display[y][x];
			buffer[(y * SCREEN_WIDTH) + x] = (0xFFFFFF00 * pixel) | 0x000000FF;
		}
	}
}

void window_update(struct sdl_window* win, uint32_t* buffer)
{
	SDL_UpdateTexture(win->gTexture, NULL, buffer, SCREEN_HEIGHT * sizeof(uint32_t));
	SDL_RenderClear(win->gRenderer);
	SDL_RenderCopy(win->gRenderer, win->gTexture, NULL, NULL);
	SDL_RenderPresent(win->gRenderer);
}

void window_close(struct sdl_window* win)
{
	SDL_DestroyWindow(win->gWindow);
	SDL_DestroyRenderer(win->gRenderer);
	SDL_DestroyTexture(win->gTexture);
	SDL_Quit();
}