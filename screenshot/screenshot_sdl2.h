#ifndef XBOX_SCREENSHOT_SDL2_H
#define XBOX_SCREENSHOT_SDL2_H

#include <SDL.h>

int XboxScreenshot_SDL2_SaveBMP(SDL_Renderer *renderer, const char *outPath);

#endif
