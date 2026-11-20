// Bare-basic saving template for NXDK. It makes sure the E: drive is ready, creates the save folders under E:\UDATA\<TitleID>\<Subdir>, writes the title and save names in the special text format (UTF-16LE) the Xbox expects, copies a save image file from D:\assets, and then quick-reboots the console.

#include <hal/debug.h>
#include <hal/video.h>
// This is our bread and butter. We need to make sure E is mounted before the save is loaded or written!
#include <nxdk/mount.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <stdio.h>

// Your title ID. If porting an existing game, I'd recommend seeing who published existing ports and using two ASCII characters of the publisher name followed by an incremental game number that doesn't already exist
// From ConsoleMods' explanation of how title IDs work:
// "It is usually composed of 2 characters for the publisher, followed by an incremental game number (3 numbers for original Xbox, above 2000 for non-original Xbox games).
// For example, for Microsoft's first original Xbox game, Oddworld: Munch's Oddysee, the Title ID is MS-001: MS for Microsoft, and 001 because it is their first game."

#define TITLE_ID        "SAVETEST" 
// Your game name. This stays static usually, but you COULD modify it on the fly if you wanted. Saves as TitleMeta.xbx containing "Name=TITLE_NAME".
#define TITLE_NAME      "Generic NXDK Save"
// It's recommended to drop the actual save data into a subdirectory. This can be used for multiple profiles, but also makes individual saves editable.
#define SAVE_SUBDIR     "slot_01"
// The name of your save. For example, "Chapter 1 - The Beginning". If you can fetch this from your game engine, you don't have to bother manually naming saves! Saves as SaveMeta.xbx containing "Name=SAVE_NAME".
#define SAVE_NAME       "Save 1"
// Source images for your title (usually close-up cover art) and save images (usually level images or icons generally signifying progress). 
#define TITLE_IMAGE_XBX   "D:\\assets\\TitleImage.xbx"
#define SAVE_IMAGE_XBX   "D:\\assets\\SaveImage.xbx"

static void WriteUtf16XbX(const char *path, const char *key, const char *val)
{
	char line[256];
	unsigned char buf[516];
	int len, n = 0, i;
	FILE *f = fopen(path, "wb");
	if (!f) return;

	len = snprintf(line, sizeof(line), "%s=%s\r\n", key, val);
	buf[n++] = 0xFF;
	buf[n++] = 0xFE;
	for (i = 0; i < len && n + 2 < (int)sizeof(buf); i++)
	{
		buf[n++] = (unsigned char)line[i];
		buf[n++] = 0x00;
	}
	fwrite(buf, 1, (size_t)n, f);
	fclose(f);
}

static void CopyFileSimple(const char *src, const char *dst)
{
	unsigned char buf[4096];
	size_t r;
	FILE *in = fopen(src, "rb");
	FILE *out;
	if (!in) return;

	out = fopen(dst, "wb");
	if (!out) { fclose(in); return; }

	while ((r = fread(buf, 1, sizeof(buf), in)) > 0)
		fwrite(buf, 1, r, out);

	fclose(in);
	fclose(out);
}

void SaveXboxData(const char *titleId, const char *titleName, const char *subdir, const char *saveName, const char *srcImageXbx)
{
	char titleDir[260], saveDir[260], path[260];
  // Check if E is mounted and, if not, mount it.
	if (GetFileAttributesA("E:\\") == INVALID_FILE_ATTRIBUTES)
		nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
  // Checks if your title ID's save folder exists and, if not, creates it.
	snprintf(titleDir, sizeof(titleDir), "E:\\UDATA\\%s", titleId);
	CreateDirectoryA(titleDir, NULL);
  // Checks if your title ID's save subfolder folder exists and, if not, creates it.
	snprintf(saveDir, sizeof(saveDir), "%s\\%s", titleDir, subdir);
	CreateDirectoryA(saveDir, NULL);
  // Writes the contents of TitleMeta.xbx into your title ID folder
	snprintf(path, sizeof(path), "%s\\TitleMeta.xbx", titleDir);
	WriteUtf16XbX(path, "TitleName", titleName);
  // Copies TitleImage.xbx into your title ID's subfolder
	snprintf(path, sizeof(path), "%s\\TitleImage.xbx", titleDir);
	CopyFileSimple(srcImageXbx, path);
  // Writes the contents of SaveMeta.xbx into your title ID's subfolder
	snprintf(path, sizeof(path), "%s\\SaveMeta.xbx", saveDir);
	WriteUtf16XbX(path, "Name", saveName);
  // Copies TitleImage.xbx into your title ID's subfolder
	snprintf(path, sizeof(path), "%s\\SaveImage.xbx", saveDir);
	CopyFileSimple(srcImageXbx, path);
}

void main(void)
{
	XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
	SaveXboxData(TITLE_ID, TITLE_NAME, SAVE_SUBDIR, SAVE_NAME, SRC_IMAGE_XBX);
	debugPrint("Done. Rebooting...\n");
	Sleep(REBOOT_DELAY_MS);
	HalReturnToFirmware(HalQuickRebootRoutine);
}
