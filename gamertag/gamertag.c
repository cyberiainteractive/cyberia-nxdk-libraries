#include "gamertag.h"

#include <hal/debug.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#ifndef XBOX_ACCOUNT_FIRST_SECTOR
#define XBOX_ACCOUNT_FIRST_SECTOR 12
#endif

#ifndef XBOX_ACCOUNT_SECTORS
#define XBOX_ACCOUNT_SECTORS 8
#endif

#ifndef XBOX_ACCOUNT_GAMERTAG_OFFSET
#define XBOX_ACCOUNT_GAMERTAG_OFFSET 0x18
#endif

#ifndef XBOX_ACCOUNT_GAMERTAG_BYTES
#define XBOX_ACCOUNT_GAMERTAG_BYTES 16
#endif

#define XBOX_SECTOR_SIZE 512U

static unsigned int ExtractPrintableTag(
	const unsigned char *raw,
	unsigned int maxBytes,
	char *outBuf,
	unsigned int outSize)
{
	unsigned int i;
	unsigned int o = 0;
	unsigned int limit = (maxBytes > 0) ? (maxBytes - 1U) : 0U;

	if (!raw || !outBuf || outSize <= 1U)
		return 0;

	for (i = 0; i < limit && raw[i] != 0x00; i++)
	{
		if (raw[i] < 0x20 || raw[i] >= 0x7F)
			return 0;

		if (raw[i] == '"' || raw[i] == '\\' || raw[i] == ';')
			continue;

		if (o + 1U < outSize)
			outBuf[o++] = (char)raw[i];
	}

	outBuf[o] = '\0';
	return o;
}

int XboxLiveProfile_GetFirstGamertag(char *outGamertag, unsigned int outSize)
{
	OBJECT_ATTRIBUTES objAttr;
	ANSI_STRING devName;
	IO_STATUS_BLOCK ioStatus;
	LARGE_INTEGER offset;
	HANDLE hDisk = NULL;
	NTSTATUS status;
	static unsigned char sectorBuf[XBOX_ACCOUNT_SECTORS * XBOX_SECTOR_SIZE] __attribute__((aligned(512)));
	unsigned int sector;

	if (!outGamertag || outSize == 0)
		return 0;

	outGamertag[0] = '\0';

	RtlInitAnsiString(&devName, "\\Device\\Harddisk0\\Partition0");
	InitializeObjectAttributes(&objAttr, &devName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = NtOpenFile(
		&hDisk,
		FILE_GENERIC_READ,
		&objAttr,
		&ioStatus,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_NO_INTERMEDIATE_BUFFERING | FILE_SYNCHRONOUS_IO_NONALERT);

	if (!NT_SUCCESS(status) || !hDisk)
	{
		debugPrint("XboxLiveProfile: NtOpenFile failed (0x%08lX)\n", (unsigned long)status);
		return 0;
	}

	memset(sectorBuf, 0, sizeof(sectorBuf));
	offset.QuadPart = (LONGLONG)XBOX_ACCOUNT_FIRST_SECTOR * (LONGLONG)XBOX_SECTOR_SIZE;

	status = NtReadFile(
		hDisk,
		NULL,
		NULL,
		NULL,
		&ioStatus,
		sectorBuf,
		sizeof(sectorBuf),
		&offset);

	NtClose(hDisk);

	if (!NT_SUCCESS(status))
	{
		debugPrint("XboxLiveProfile: NtReadFile failed (0x%08lX)\n", (unsigned long)status);
		return 0;
	}

	for (sector = 0; sector < XBOX_ACCOUNT_SECTORS; sector++)
	{
		unsigned int entryOffset = (sector * XBOX_SECTOR_SIZE) + XBOX_ACCOUNT_GAMERTAG_OFFSET;
		const unsigned char *tag = sectorBuf + entryOffset;

		if (entryOffset >= ioStatus.Information)
			break;

		if (ExtractPrintableTag(tag, XBOX_ACCOUNT_GAMERTAG_BYTES, outGamertag, outSize) > 0)
			return 1;
	}

	outGamertag[0] = '\0';
	return 0;
}
