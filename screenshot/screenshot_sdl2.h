#ifndef XBOX_SCREENSHOT_SDL2_H
#define XBOX_SCREENSHOT_SDL2_H

#include <SDL.h>

#ifndef XBOX_SCREENSHOT_DIR
#define XBOX_SCREENSHOT_DIR "E:\\Screenshots"
#endif

#ifndef XBOX_SCREENSHOT_PREFIX
#define XBOX_SCREENSHOT_PREFIX "screenshot"
#endif

void XboxScreenshot_SDL2_SetIndex(unsigned int nextIndex);

unsigned int XboxScreenshot_SDL2_GetIndex(void);

int XboxScreenshot_SDL2_CaptureNext(
	SDL_Renderer *renderer,
	char *outPathBuf,
	unsigned int outPathBufSize);

#endif
