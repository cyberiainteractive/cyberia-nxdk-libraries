#include "xbox_save.h"

#include <hal/debug.h>
#include <nxdk/mount.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

static int WriteUtf16XbX(const char *path, const char *key, const char *val)
{
	char line[256];
	unsigned char buf[520];
	int len, n = 0, i;
	FILE *f;

	if (!path || !key || !val)
		return 0;

	f = fopen(path, "wb");
	if (!f)
		return 0;

	len = snprintf(line, sizeof(line), "%s=%s\r\n", key, val);
	if (len < 0)
	{
		fclose(f);
		return 0;
	}

	buf[n++] = 0xFF;
	buf[n++] = 0xFE;
	for (i = 0; i < len && (n + 2) < (int)sizeof(buf); i++)
	{
		buf[n++] = (unsigned char)line[i];
		buf[n++] = 0x00;
	}

	fwrite(buf, 1, (size_t)n, f);
	fclose(f);
	return 1;
}

static int CopyFileSimple(const char *src, const char *dst)
{
	unsigned char buf[4096];
	size_t r;
	FILE *in, *out;

	if (!src || !dst)
		return 0;

	in = fopen(src, "rb");
	if (!in)
		return 0;

	out = fopen(dst, "wb");
	if (!out)
	{
		fclose(in);
		return 0;
	}

	while ((r = fread(buf, 1, sizeof(buf), in)) > 0)
	{
		if (fwrite(buf, 1, r, out) != r)
		{
			fclose(in);
			fclose(out);
			return 0;
		}
	}

	fclose(in);
	fclose(out);
	return 1;
}

int XboxSave_Write(
	const char *titleId,
	const char *titleName,
	const char *subdir,
	const char *saveName,
	const char *titleImageXbx,
	const char *saveImageXbx)
{
	char udataDir[260];
	char titleDir[260];
	char saveDir[260];
	char path[260];

	const char *tid = (titleId && titleId[0]) ? titleId : XBOX_SAVE_DEFAULT_TITLE_ID;
	const char *tname = (titleName && titleName[0]) ? titleName : XBOX_SAVE_DEFAULT_TITLE_NAME;
	const char *sub = (subdir && subdir[0]) ? subdir : XBOX_SAVE_DEFAULT_SUBDIR;
	const char *sname = (saveName && saveName[0]) ? saveName : XBOX_SAVE_DEFAULT_SAVE_NAME;
	const char *timg = (titleImageXbx && titleImageXbx[0]) ? titleImageXbx : XBOX_SAVE_DEFAULT_TITLE_IMAGE;
	const char *simg = (saveImageXbx && saveImageXbx[0]) ? saveImageXbx : XBOX_SAVE_DEFAULT_SAVE_IMAGE;

	if (GetFileAttributesA("E:\\") == INVALID_FILE_ATTRIBUTES)
	{
		if (FAILED(nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\")))
		{
			debugPrint("XboxSave_Write: failed to mount E:\\\n");
			return 0;
		}
	}

	snprintf(udataDir, sizeof(udataDir), "E:\\UDATA");
	CreateDirectoryA(udataDir, NULL);

	snprintf(titleDir, sizeof(titleDir), "%s\\%s", udataDir, tid);
	CreateDirectoryA(titleDir, NULL);

	snprintf(saveDir, sizeof(saveDir), "%s\\%s", titleDir, sub);
	CreateDirectoryA(saveDir, NULL);

	snprintf(path, sizeof(path), "%s\\TitleMeta.xbx", titleDir);
	if (!WriteUtf16XbX(path, "TitleName", tname))
	{
		debugPrint("XboxSave_Write: failed writing %s\n", path);
		return 0;
	}

	snprintf(path, sizeof(path), "%s\\TitleImage.xbx", titleDir);
	CopyFileSimple(timg, path);

	snprintf(path, sizeof(path), "%s\\SaveMeta.xbx", saveDir);
	if (!WriteUtf16XbX(path, "Name", sname))
	{
		debugPrint("XboxSave_Write: failed writing %s\n", path);
		return 0;
	}

	snprintf(path, sizeof(path), "%s\\SaveImage.xbx", saveDir);
	CopyFileSimple(simg, path);

	return 1;
}
