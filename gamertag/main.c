#include "gamertag.h"

#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <SDL.h>
#include <stdio.h>

void main(void)
{
	char gamertag[XBOX_LIVE_GAMERTAG_MAX_LEN + 1];
	int found;
	int running = 1;
	SDL_Window *win = NULL;
	SDL_Renderer *ren = NULL;

	XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

	found = XboxLiveProfile_GetFirstGamertag(gamertag, sizeof(gamertag));
	if (found)
	{
		debugPrint("First Xbox Live Profile: %s\n", gamertag);
	}
	else
	{
		snprintf(gamertag, sizeof(gamertag), "None found");
		debugPrint("No Xbox Live profile found on disk.\n");
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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

	while (running)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
				running = 0;
		}

		if (found)
			SDL_SetRenderDrawColor(ren, 16, 72, 16, 255);
		else
			SDL_SetRenderDrawColor(ren, 24, 24, 32, 255);

		SDL_RenderClear(ren);
		SDL_RenderPresent(ren);

		SDL_Delay(16);
	}

	if (ren) SDL_DestroyRenderer(ren);
	if (win) SDL_DestroyWindow(win);
	SDL_Quit();
}
