#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <Wdm.h>

#include "main.h"

#define POINTER_ONE_OFFSET 0x69463C /* Offset to nt!EtwpLastBranchLookAsideList + 0x180 */
#define POINTER_TWO_OFFSET 0x18
#define COLOR_ADDRESS_OFFSET 0x28

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _BUG_CHECK_INFO
{
	BYTE __pad[39];
	ULONG BackgroundColor;
 } BUG_CHECK_INFO, * PBUG_CHECK_INFO;

__forceinline wchar_t locase_w(wchar_t c)
{
	if ((c >= 'A') && (c <= 'Z'))
		return c + 0x20;
	else
		return c;
}

NTSTATUS
ModifyBlueScreenBackground(void* lpNtoskrnlBaseAddr, ULONG ntImageSize, ULONG color);

ULONG
GetOffsetFromPattern(const void* lpBaseAddr, const ULONG baseAddrSize, const void* lpPattern, const ULONG patternLength);

void* GetKernelBaseAddr(PDRIVER_OBJECT DriverObject, ULONG* imageSize);