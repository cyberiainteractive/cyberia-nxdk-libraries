// NXDK + SDL2 In-Game Reset template

#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <SDL.h>
#include <stdio.h>

#define IGR_REBOOT_DELAY_MS  1000

typedef struct IGR_StateSDL2
{
	int triggered;
} IGR_StateSDL2;

static void IGR_SDL2_Init(IGR_StateSDL2 *state)
{
	if (!state)
		return;
	state->triggered = 0;
}

static int IGR_SDL2_Update(IGR_StateSDL2 *state)
{
	int numJoysticks;

	if (!state)
		return 0;

	if (state->triggered)
		return 1;

	numJoysticks = SDL_NumJoysticks();
	for (int i = 0; i < numJoysticks; i++)
	{
		SDL_GameController *pad;
		float lt, rt;
		Uint8 back, start;

		if (!SDL_IsGameController(i))
			continue;

		pad = SDL_GameControllerOpen(i);
		if (!pad)
			continue;

		lt = (float)SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f;
		rt = (float)SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;
		back = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK);
		start = SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START);

		SDL_GameControllerClose(pad);

		if (back && start && lt > 0.0f && rt > 0.0f)
		{
			state->triggered = 1;
			break;
		}
	}

	return state->triggered;
}

static void IGR_SDL2_Reboot(void)
{
	debugPrint("IGR triggered, goodbye!\n");
	SDL_Quit();
	Sleep(IGR_REBOOT_DELAY_MS);
	HalReturnToFirmware(HalQuickRebootRoutine);
}

void main(void)
{
	IGR_StateSDL2 igr;
	int running = 1;

	XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		debugPrint("SDL_Init failed: %s\n", SDL_GetError());
		while (1) Sleep(1000);
	}

	IGR_SDL2_Init(&igr);
	debugPrint("Press Back + Start + LT + RT to reboot.\n");

	while (running)
	{
		SDL_Event ev;

		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
				running = 0;
		}

		if (IGR_SDL2_Update(&igr))
		{
			running = 0;
		}

		SDL_Delay(16);
	}

	if (igr.triggered)
	{
		IGR_SDL2_Reboot();
	}

	SDL_Quit();
}
