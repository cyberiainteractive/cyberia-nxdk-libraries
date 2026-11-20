#ifndef IGR_SDL3_H
#define IGR_SDL3_H

#ifndef IGR_REBOOT_DELAY_MS
#define IGR_REBOOT_DELAY_MS 250
#endif

typedef struct IGR_StateSDL3
{
	int triggered;
} IGR_StateSDL3;

void IGR_SDL3_Init(IGR_StateSDL3 *state);
int IGR_SDL3_Update(IGR_StateSDL3 *state);
void IGR_SDL3_Reboot(void);

#endif
