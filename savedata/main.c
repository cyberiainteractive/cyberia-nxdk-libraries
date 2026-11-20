#include "xbox_save.h"

#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>

#define REBOOT_DELAY_MS 1500

void main(void)
{
	int ok;

	XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

	ok = XboxSave_Write(
		XBOX_SAVE_DEFAULT_TITLE_ID,
		XBOX_SAVE_DEFAULT_TITLE_NAME,
		XBOX_SAVE_DEFAULT_SUBDIR,
		XBOX_SAVE_DEFAULT_SAVE_NAME,
		XBOX_SAVE_DEFAULT_TITLE_IMAGE,
		XBOX_SAVE_DEFAULT_SAVE_IMAGE);

	debugPrint(ok ? "Save written. Rebooting...\n" : "Save failed! Rebooting...\n");
	Sleep(REBOOT_DELAY_MS);
	HalReturnToFirmware(HalQuickRebootRoutine);
}
