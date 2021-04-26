#pragma once

#include <ntddk.h>
#include <wdf.h>

#include "BugCheck.h"

DRIVER_INITIALIZE DriverEntry;
#pragma alloc_text(INIT, DriverEntry)

extern void* lpNtoskrnlBaseAddress;
extern ULONG ntoskrnlImageSize;