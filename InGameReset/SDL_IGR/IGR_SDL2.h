#ifndef IGR_SDL2_H
#define IGR_SDL2_H

#ifndef IGR_REBOOT_DELAY_MS
#define IGR_REBOOT_DELAY_MS 250
#endif

typedef struct IGR_StateSDL2
{
	int triggered;
} IGR_StateSDL2;

void IGR_SDL2_Init(IGR_StateSDL2 *state);
int IGR_SDL2_Update(IGR_StateSDL2 *state);
void IGR_SDL2_Reboot(void);

#endif
