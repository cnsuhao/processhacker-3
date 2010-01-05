#ifndef NTIMPORT_H
#define NTIMPORT_H

#include <ntbasic.h>
#include <ntexapi.h>
#include <ntioapi.h>
#include <ntmmapi.h>
#include <ntobapi.h>
#include <ntpsapi.h>
#include <ntseapi.h>
#include <ntrtl.h>

#ifdef NTIMPORT_PRIVATE
#define EXT
#define EQNULL = NULL
#else
#define EXT extern
#define EQNULL
#endif

EXT _NtAlertResumeThread NtAlertResumeThread EQNULL;
EXT _NtAlertThread NtAlertThread EQNULL;
EXT _NtAllocateVirtualMemory NtAllocateVirtualMemory EQNULL;
EXT _NtClose NtClose EQNULL;
EXT _NtCreateFile NtCreateFile EQNULL;
EXT _NtDeleteFile NtDeleteFile EQNULL;
EXT _NtDeviceIoControlFile NtDeviceIoControlFile EQNULL;
EXT _NtDuplicateObject NtDuplicateObject EQNULL;
EXT _NtFreeVirtualMemory NtFreeVirtualMemory EQNULL;
EXT _NtFsControlFile NtFsControlFile EQNULL;
EXT _NtGetNextProcess NtGetNextProcess EQNULL;
EXT _NtGetNextThread NtGetNextThread EQNULL;
EXT _NtLoadDriver NtLoadDriver EQNULL;
EXT _NtOpenFile NtOpenFile EQNULL;
EXT _NtOpenProcess NtOpenProcess EQNULL;
EXT _NtOpenProcessToken NtOpenProcessToken EQNULL;
EXT _NtOpenThread NtOpenThread EQNULL;
EXT _NtProtectVirtualMemory NtProtectVirtualMemory EQNULL;
EXT _NtQueryInformationProcess NtQueryInformationProcess EQNULL;
EXT _NtQueryInformationThread NtQueryInformationThread EQNULL;
EXT _NtQueryInformationToken NtQueryInformationToken EQNULL;
EXT _NtQueryObject NtQueryObject EQNULL;           
EXT _NtQuerySection NtQuerySection EQNULL;
EXT _NtQuerySystemInformation NtQuerySystemInformation EQNULL;
EXT _NtQueryVirtualMemory NtQueryVirtualMemory EQNULL;
EXT _NtQueueApcThread NtQueueApcThread EQNULL;
EXT _NtReadFile NtReadFile EQNULL;
EXT _NtReadVirtualMemory NtReadVirtualMemory EQNULL;
EXT _NtResumeProcess NtResumeProcess EQNULL;
EXT _NtResumeThread NtResumeThread EQNULL; 
EXT _NtSetInformationProcess NtSetInformationProcess EQNULL;
EXT _NtSetInformationThread NtSetInformationThread EQNULL;
EXT _NtSetInformationToken NtSetInformationToken EQNULL;
EXT _NtSuspendProcess NtSuspendProcess EQNULL;
EXT _NtSuspendThread NtSuspendThread EQNULL;
EXT _NtTerminateProcess NtTerminateProcess EQNULL;
EXT _NtTerminateThread NtTerminateThread EQNULL;
EXT _NtUnloadDriver NtUnloadDriver EQNULL;
EXT _NtWaitForSingleObject NtWaitForSingleObject EQNULL;
EXT _NtWriteFile NtWriteFile EQNULL;
EXT _NtWriteVirtualMemory NtWriteVirtualMemory EQNULL;
EXT _RtlCreateQueryDebugBuffer RtlCreateQueryDebugBuffer EQNULL;
EXT _RtlDestroyQueryDebugBuffer RtlDestroyQueryDebugBuffer EQNULL;
EXT _RtlFindMessage RtlFindMessage EQNULL;
EXT _RtlMultiByteToUnicodeN RtlMultiByteToUnicodeN EQNULL;
EXT _RtlMultiByteToUnicodeSize RtlMultiByteToUnicodeSize EQNULL;
EXT _RtlNtStatusToDosError RtlNtStatusToDosError EQNULL;
EXT _RtlQueryProcessDebugInformation RtlQueryProcessDebugInformation EQNULL;
EXT _RtlUnicodeToMultiByteN RtlUnicodeToMultiByteN EQNULL;
EXT _RtlUnicodeToMultiByteSize RtlUnicodeToMultiByteSize EQNULL;

BOOLEAN PhInitializeImports();

#endif
