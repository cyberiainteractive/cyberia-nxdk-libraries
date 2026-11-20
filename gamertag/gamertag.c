#include "gamertag.h"

#include <hal/debug.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#define FATX_MAGIC            0x58544146U
#define FATX_ACCOUNTS_OFFSET  0x50U
#define FATX_ACCOUNT_SIZE     0x6CU
#define FATX_ACCOUNT_COUNT    8U
#define ACCOUNT_XUID_OFFSET   0x00U
#define ACCOUNT_GT_OFFSET     0x0CU
#define ACCOUNT_GT_LEN        16U

static int IsPrintableGamertag(const unsigned char *raw, unsigned int maxLen)
{
	unsigned int i;
	int hasChar = 0;

	for (i = 0; i < maxLen; i++)
	{
		if (raw[i] == 0x00)
			return hasChar;
		if (raw[i] < 0x20 || raw[i] > 0x7E)
			return 0;
		hasChar = 1;
	}

	return 0;
}

int XboxLiveProfile_GetFirstGamertag(char *outGamertag, unsigned int outSize)
{
	OBJECT_ATTRIBUTES objAttr;
	UNICODE_STRING devName;
	IO_STATUS_BLOCK ioStatus;
	LARGE_INTEGER offset;
	HANDLE hDisk = NULL;
	NTSTATUS status;
	unsigned char sectorBuf[1024];
	unsigned int slot;

	if (!outGamertag || outSize == 0)
		return 0;

	outGamertag[0] = '\0';

	RtlInitAnsiString((PANSI_STRING)&devName, "\\Device\\Harddisk0\\Partition0");
	InitializeObjectAttributes(&objAttr, &devName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = NtOpenFile(
		&hDisk,
		FILE_READ_DATA | SYNCHRONIZE,
		&objAttr,
		&ioStatus,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_SYNCHRONOUS_IO_NONALERT);

	if (!NT_SUCCESS(status) || !hDisk)
	{
		debugPrint("XboxLiveProfile: NtOpenFile failed (0x%08lX)\n", (unsigned long)status);
		return 0;
	}

	memset(sectorBuf, 0, sizeof(sectorBuf));
	offset.QuadPart = 0;

	status = NtReadFile(
		hDisk,
		NULL,
		NULL,
		NULL,
		&ioStatus,
		sectorBuf,
		sizeof(sectorBuf),
		&offset,
		NULL);

	NtClose(hDisk);

	if (!NT_SUCCESS(status) || ioStatus.Information < (FATX_ACCOUNTS_OFFSET + FATX_ACCOUNT_COUNT * FATX_ACCOUNT_SIZE))
	{
		debugPrint("XboxLiveProfile: NtReadFile failed (0x%08lX)\n", (unsigned long)status);
		return 0;
	}

	if (*(const unsigned int *)(sectorBuf + 0x00) != FATX_MAGIC)
	{
		debugPrint("XboxLiveProfile: FATX magic not found\n");
		return 0;
	}

	for (slot = 0; slot < FATX_ACCOUNT_COUNT; slot++)
	{
		const unsigned char *acct = sectorBuf + FATX_ACCOUNTS_OFFSET + (slot * FATX_ACCOUNT_SIZE);
		const unsigned char *gtRaw = acct + ACCOUNT_GT_OFFSET;
		unsigned long long xuid = *(const unsigned long long *)(acct + ACCOUNT_XUID_OFFSET);

		if (xuid == 0ULL)
			continue;

		if (IsPrintableGamertag(gtRaw, ACCOUNT_GT_LEN))
		{
			unsigned int len = (unsigned int)strlen((const char *)gtRaw);
			if (len >= outSize)
				len = outSize - 1;

			memcpy(outGamertag, gtRaw, len);
			outGamertag[len] = '\0';
			return 1;
		}
	}

	return 0;
}
