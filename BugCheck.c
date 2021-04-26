#include "BugCheck.h"

int _strcmpi_w(const wchar_t* s1, const wchar_t* s2)
{
	wchar_t c1, c2;

	if (s1 == s2)
		return 0;

	if (s1 == 0)
		return -1;

	if (s2 == 0)
		return 1;

	do {
		c1 = locase_w(*s1);
		c2 = locase_w(*s2);
		s1++;
		s2++;
	} while ((c1 != 0) && (c1 == c2));

	return (int)(c1 - c2);
}

void* GetKernelBaseAddr(PDRIVER_OBJECT DriverObject, ULONG* imageSize)
{
	PLDR_DATA_TABLE_ENTRY entry = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY first = entry;
	while ((PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != first)
	{
		if (_strcmpi_w(entry->BaseDllName.Buffer, L"ntoskrnl.exe") == 0)
		{
			*imageSize = entry->SizeOfImage;
			return entry->DllBase;
		}
		entry = (PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
	}
	return 0;
}

ULONG
GetOffsetFromPattern(const void* lpBaseAddr, const ULONG baseAddrSize, const void* lpPattern, const ULONG patternLength)
{
	ULONG offset = 0;
	int result = 0;

	while (offset <= baseAddrSize - patternLength)
	{
		result = memcmp((void*)((char*)lpBaseAddr + offset), lpPattern, patternLength);
		if (result == 0)
			return offset;

		offset += 1;
	}

	return 0;
}

NTSTATUS
ModifyBlueScreenBackground(void *lpNtoskrnlBaseAddr, ULONG ntImageSize, ULONG color)
{
	unsigned __int64* lpBgpFwDisplayBugCheckScreen = NULL;
	ULONG BgpFwDisplayBugCheckScreenOffset = 0;
	unsigned __int64* lpBgpCriticalMode_s = NULL;
	unsigned __int64* lpBugCheckInfo_s = NULL;
	unsigned __int64* lpColorAddr = NULL;

	// Signature for BgpFwDisplayBugCheckScreen
	const void* lpBgpFwDisplayBugCheckScreenSig = "\x48\x8b\xc4\x48\x89\x58\x08\x48\x89\x68\x10\x48\x89\x70\x20\x4c\x89\x40\x18\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x83\xec\x50\x48\x83\x60\xc0\x00\x8b\xe9\x48\x83\x60\xb8\x00\x4d\x8b\xe1\x8b\x05\x07\x3a\x65\x00\x4c\x8b\xea\xb9\x00\x00\x40\x00\xa8\x04\x74\x25\x85\xc1";
	const ULONG sigLength = 67;

	/*
		Find the offset to BgpFwDisplayBugCheckScreen (a launch point for our offsets)
	*/
	BgpFwDisplayBugCheckScreenOffset = GetOffsetFromPattern(lpNtoskrnlBaseAddr, ntImageSize, lpBgpFwDisplayBugCheckScreenSig, sigLength);
	if (BgpFwDisplayBugCheckScreenOffset == 0)
	{
		DbgPrintEx(0, 0, "[-] Offset not found\n");
		return STATUS_NOT_FOUND;
	}
	else
		DbgPrintEx(0, 0, "[+] BgpFwDisplayBugCheckScreen offset found: %X\n", BgpFwDisplayBugCheckScreenOffset);

	/*
		Aquire BgpFwDisplayBugCheckScreen address
	*/
	lpBgpFwDisplayBugCheckScreen = (unsigned __int64*)((char*)lpNtoskrnlBaseAddr + BgpFwDisplayBugCheckScreenOffset);
	DbgPrintEx(0, 0, "[+] BgpFwDisplayBugCheckScreen found: %p\n", lpBgpFwDisplayBugCheckScreen);

	/*
		Aquire lpBgpCriticalMode_s address
	*/
	lpBgpCriticalMode_s = (unsigned __int64*)((char*)lpBgpFwDisplayBugCheckScreen + POINTER_ONE_OFFSET);
	DbgPrintEx(0, 0, "[+] lpBgpCriticalMode_s found: %p\n", lpBgpCriticalMode_s);

	/*
		Aquire lpBugCheckInfo_s address
	*/
	lpBugCheckInfo_s = (unsigned __int64*)((char*)*lpBgpCriticalMode_s + POINTER_TWO_OFFSET);
	DbgPrintEx(0, 0, "[+] lpBugCheckInfo_s found: %p\n", lpBugCheckInfo_s);

	/*
		Aquire BSOD background color address
	*/
	lpColorAddr = (unsigned __int64*)((char*)*lpBugCheckInfo_s + COLOR_ADDRESS_OFFSET);
	DbgPrintEx(0, 0, "[+] lpColorAddr found: %p: Value %0x\n", lpColorAddr, (ULONG)*lpColorAddr);

	/*
		Write new color value to color address
	*/
	memcpy(lpColorAddr, &color, sizeof(ULONG));

	DbgPrintEx(0, 0, "[+] Wrote color %X to %p: New value %0x\n", color, lpColorAddr, (ULONG)*lpColorAddr);

	return STATUS_SUCCESS;
}