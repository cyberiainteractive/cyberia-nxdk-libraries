#ifndef XBOX_SAVE_H
#define XBOX_SAVE_H

#define XBOX_SAVE_DEFAULT_TITLE_ID     "SAVETEST"
#define XBOX_SAVE_DEFAULT_TITLE_NAME   "Generic NXDK Save"
#define XBOX_SAVE_DEFAULT_SUBDIR       "slot_01"
#define XBOX_SAVE_DEFAULT_SAVE_NAME    "Save 1"
#define XBOX_SAVE_DEFAULT_TITLE_IMAGE  "D:\\assets\\TitleImage.xbx"
#define XBOX_SAVE_DEFAULT_SAVE_IMAGE   "D:\\assets\\SaveImage.xbx"

int XboxSave_Write(
	const char *titleId,
	const char *titleName,
	const char *subdir,
	const char *saveName,
	const char *titleImageXbx,
	const char *saveImageXbx);

#endif
