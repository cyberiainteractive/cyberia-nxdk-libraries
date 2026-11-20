#include "screenshot_sdl2.h"

#include <hal/debug.h>
#include <nxdk/mount.h>
#include <windows.h>
#include <stdio.h>

static unsigned int g_shotIndex = 1;
static int g_comboHeldLastFrame = 0;

static int IsButtonPressed(SDL_GameController *pad, SDL_GameControllerButton btn)
{
	if (!pad || btn == SDL_CONTROLLER_BUTTON_INVALID)
		return 1;

	return SDL_GameControllerGetButton(pad, btn) != 0;
}

static int IsComboActiveOnAnyController(void)
{
	int numJoysticks = SDL_NumJoysticks();

	for (int i = 0; i < numJoysticks; i++)
	{
		SDL_GameController *pad;
		int pressed;

		if (!SDL_IsGameController(i))
			continue;

		pad = SDL_GameControllerOpen(i);
		if (!pad)
			continue;

		pressed =
			IsButtonPressed(pad, XBOX_SCREENSHOT_BTN_1) &&
			IsButtonPressed(pad, XBOX_SCREENSHOT_BTN_2) &&
			IsButtonPressed(pad, XBOX_SCREENSHOT_BTN_3);

		SDL_GameControllerClose(pad);

		if (pressed)
			return 1;
	}

	return 0;
}

static int EnsureStorageReady(void)
{
	if (GetFileAttributesA("E:\\") == INVALID_FILE_ATTRIBUTES)
	{
		if (FAILED(nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\")))
		{
			debugPrint("failed to mount E:\\\n");
			return 0;
		}
	}

	CreateDirectoryA(XBOX_SCREENSHOT_DIR, NULL);
	if (GetFileAttributesA(XBOX_SCREENSHOT_DIR) == INVALID_FILE_ATTRIBUTES)
	{
		debugPrint("failed to access directory %s\n", XBOX_SCREENSHOT_DIR);
		return 0;
	}

	return 1;
}

void XboxScreenshot_SDL2_SetIndex(unsigned int nextIndex)
{
	g_shotIndex = (nextIndex == 0) ? 1 : nextIndex;
}

unsigned int XboxScreenshot_SDL2_GetIndex(void)
{
	return g_shotIndex;
}

int XboxScreenshot_SDL2_CaptureNext(
	SDL_Renderer *renderer,
	char *outPathBuf,
	unsigned int outPathBufSize)
{
	int w = 0, h = 0;
	char filePath[260];
	SDL_Surface *surface = NULL;

	if (!renderer)
		return 0;

	if (!EnsureStorageReady())
		return 0;

	if (SDL_GetRendererOutputSize(renderer, &w, &h) != 0 || w <= 0 || h <= 0)
	{
		debugPrint("invalid renderer size: %s\n", SDL_GetError());
		return 0;
	}

	snprintf(
		filePath,
		sizeof(filePath),
		"%s\\%s_%04u.bmp",
		XBOX_SCREENSHOT_DIR,
		XBOX_SCREENSHOT_PREFIX,
		g_shotIndex);

	surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
	if (!surface)
	{
		debugPrint("SDL_CreateRGBSurfaceWithFormat failed: %s\n", SDL_GetError());
		return 0;
	}

	if (SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch) != 0)
	{
		debugPrint("SDL_RenderReadPixels failed: %s\n", SDL_GetError());
		SDL_FreeSurface(surface);
		return 0;
	}

	if (SDL_SaveBMP(surface, filePath) != 0)
	{
		debugPrint("SDL_SaveBMP failed for %s: %s\n", filePath, SDL_GetError());
		SDL_FreeSurface(surface);
		return 0;
	}

	SDL_FreeSurface(surface);

	if (outPathBuf && outPathBufSize > 0)
	{
		snprintf(outPathBuf, outPathBufSize, "%s", filePath);
	}

	g_shotIndex++;
	return 1;
}

int XboxScreenshot_SDL2_UpdateAndCapture(
	SDL_Renderer *renderer,
	char *outPathBuf,
	unsigned int outPathBufSize)
{
	int comboNow = IsComboActiveOnAnyController();
	int justPressed = (comboNow && !g_comboHeldLastFrame);

	g_comboHeldLastFrame = comboNow;

	if (!justPressed)
		return 0;

	return XboxScreenshot_SDL2_CaptureNext(renderer, outPathBuf, outPathBufSize);
}
