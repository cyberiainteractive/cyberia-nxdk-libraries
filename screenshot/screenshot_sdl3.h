#ifndef XBOX_SCREENSHOT_SDL3_H
#define XBOX_SCREENSHOT_SDL3_H

#include <SDL3/SDL.h>

int XboxScreenshot_SDL3_SaveBMP(SDL_Renderer *renderer, const char *outPath);

#endif
