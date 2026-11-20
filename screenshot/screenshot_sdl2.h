#ifndef XBOX_SCREENSHOT_SDL2_H
#define XBOX_SCREENSHOT_SDL2_H

#include <SDL.h>

#ifndef XBOX_SCREENSHOT_DIR
#define XBOX_SCREENSHOT_DIR "E:\\Screenshots"
#endif

#ifndef XBOX_SCREENSHOT_PREFIX
#define XBOX_SCREENSHOT_PREFIX "shot"
#endif

#ifndef XBOX_SCREENSHOT_BTN_1
#define XBOX_SCREENSHOT_BTN_1 SDL_CONTROLLER_BUTTON_LEFTSTICK
#endif

#ifndef XBOX_SCREENSHOT_BTN_2
#define XBOX_SCREENSHOT_BTN_2 SDL_CONTROLLER_BUTTON_RIGHTSTICK
#endif

#ifndef XBOX_SCREENSHOT_BTN_3
#define XBOX_SCREENSHOT_BTN_3 SDL_CONTROLLER_BUTTON_INVALID
#endif

void XboxScreenshot_SDL2_SetIndex(unsigned int nextIndex);

unsigned int XboxScreenshot_SDL2_GetIndex(void);

int XboxScreenshot_SDL2_CaptureNext(
	SDL_Renderer *renderer,
	char *outPathBuf,
	unsigned int outPathBufSize);

int XboxScreenshot_SDL2_UpdateAndCapture(
	SDL_Renderer *renderer,
	char *outPathBuf,
	unsigned int outPathBufSize);

#endif
