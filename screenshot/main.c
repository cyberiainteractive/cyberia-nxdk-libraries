#include "screenshot_sdl2.h"

#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <SDL.h>
#include <stdio.h>

void main(void)
{
	SDL_Window *win = NULL;
	SDL_Renderer *ren = NULL;
	int running = 1;
	unsigned int frame = 0;

	XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		debugPrint("SDL_Init failed: %s\n", SDL_GetError());
		while (1) Sleep(1000);
	}

	if (SDL_CreateWindowAndRenderer(640, 480, 0, &win, &ren) < 0)
	{
		debugPrint("SDL_CreateWindowAndRenderer failed: %s\n", SDL_GetError());
		SDL_Quit();
		while (1) Sleep(1000);
	}

	debugPrint("Click Left Stick + Right Stick to save screenshots to %s\n", XBOX_SCREENSHOT_DIR);

	while (running)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
				running = 0;
		}

		{
			Uint8 r = (Uint8)((frame * 2) & 0xFF);
			Uint8 g = 64;
			Uint8 b = (Uint8)(160 - ((frame * 2) & 0x5F));
			SDL_Rect box = { 160, 120, 320, 240 };

			SDL_SetRenderDrawColor(ren, r, g, b, 255);
			SDL_RenderClear(ren);

			SDL_SetRenderDrawColor(ren, 235, 235, 245, 255);
			SDL_RenderDrawRect(ren, &box);
		}

		{
			char shotPath[260];
			if (XboxScreenshot_SDL2_UpdateAndCapture(ren, shotPath, sizeof(shotPath)))
			{
				debugPrint("Saved screenshot: %s\n", shotPath);
			}
		}

		SDL_RenderPresent(ren);

		frame++;
		SDL_Delay(16);
	}

	if (ren) SDL_DestroyRenderer(ren);
	if (win) SDL_DestroyWindow(win);
	SDL_Quit();
}
