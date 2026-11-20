// NXDK + NXDK-SDL3 In-Game Reset template

#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <SDL3/SDL.h>
#include <stdio.h>

#define IGR_REBOOT_DELAY_MS  1000

typedef struct IGR_StateSDL3
{
	int triggered;
} IGR_StateSDL3;

static void IGR_SDL3_Init(IGR_StateSDL3 *state)
{
	if (!state)
		return;
	state->triggered = 0;
}

static int IGR_SDL3_Update(IGR_StateSDL3 *state)
{
	int count = 0;
	SDL_JoystickID *ids;

	if (!state)
		return 0;

	if (state->triggered)
		return 1;

	ids = SDL_GetGamepads(&count);
	if (ids)
	{
		for (int i = 0; i < count; i++)
		{
			SDL_Gamepad *pad = SDL_OpenGamepad(ids[i]);
			float lt, rt;
			bool back, start;

			if (!pad)
				continue;

			lt = (float)SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) / 32767.0f;
			rt = (float)SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) / 32767.0f;
			back = SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_BACK);
			start = SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_START);

			SDL_CloseGamepad(pad);

			if (back && start && lt > 0.0f && rt > 0.0f)
			{
				state->triggered = 1;
				break;
			}
		}
		SDL_free(ids);
	}

	return state->triggered;
}

static void IGR_SDL3_Reboot(void)
{
	debugPrint("IGR triggered, goodbye!\n");
	SDL_Quit();
	Sleep(IGR_REBOOT_DELAY_MS);
	HalReturnToFirmware(HalQuickRebootRoutine);
}

void main(void)
{
	IGR_StateSDL3 igr;
	int running = 1;

	XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		debugPrint("SDL_Init failed: %s\n", SDL_GetError());
		while (1) Sleep(1000);
	}

	IGR_SDL3_Init(&igr);
	debugPrint("Press Back + Start + LT + RT to reboot.\n");

	while (running)
	{
		SDL_Event ev;

		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_EVENT_QUIT)
				running = 0;
		}

		if (IGR_SDL3_Update(&igr))
		{
			running = 0;
		}

		SDL_Delay(16);
	}

	if (igr.triggered)
	{
		IGR_SDL3_Reboot();
	}

	SDL_Quit();
}
