#include "main.h"

void* lpNtoskrnlBaseAddress = NULL;
ULONG ntoskrnlImageSize = 0;

NTSTATUS
UnloadDriver(
	PDRIVER_OBJECT lpDriverObject
);

NTSTATUS
DriverEntry(
	PDRIVER_OBJECT     lpDriverObject,
	PUNICODE_STRING    lpRegistryPath
)
{
	lpDriverObject->DriverUnload = UnloadDriver;

	UNREFERENCED_PARAMETER(lpRegistryPath);

	/* 
		Print "Hello World" for DriverEntry 
	*/
	DbgPrintEx(0, 0, "[+] BSODConfigureDriver: DriverEntry\n");

	/*
		Get base address to ntoskrnl.exe
	*/
	lpNtoskrnlBaseAddress = GetKernelBaseAddr(lpDriverObject, &ntoskrnlImageSize);
	
	/*
		Set the Blue Screen background color
		TODO: Make this an IOCTL to be called from user mode
	*/
	ULONG ulBackgroundColor = 0xFFFF0000; // Alpha == 0xFF | Red == 0xFF | Blue == 0x00 | Green == 0x00
	ModifyBlueScreenBackground(lpNtoskrnlBaseAddress, ntoskrnlImageSize, ulBackgroundColor);

	/*
		Crash the kernel, see you on the other side!
	*/
	KeBugCheck(0xD00D2BAD);

	//return STATUS_SUCCESS;
}

NTSTATUS
UnloadDriver(
	PDRIVER_OBJECT lpDriverObject
)
{
	UNREFERENCED_PARAMETER(lpDriverObject);

	// Print "Hello World" for DriverEntry
	DbgPrintEx(0, 0, "[+] BSODConfigureDriver: UnloadDriver\n");

	return STATUS_SUCCESS;
}