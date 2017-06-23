#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, VOID* reserved)
{
	OutputDebugString(TEXT("Testing output Debug string"));

	std::ifstream infile;
	char tempBuf[MAX_PATH];
	GetTempPathA(MAX_PATH, tempBuf);
	std::string tempFolder = tempBuf;
	tempFolder += "\\clientlocationx12.txt";
	infile.open(tempFolder.c_str());
	std::string startDirectory;
	std::string exePath;

	if (infile.is_open())
	{
		while (!infile.eof())
		{
			std::getline(infile, startDirectory);
			break;
		}
	}

	infile.close();

	exePath += startDirectory + "\\tutclient.exe";

	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:

			STARTUPINFOA lpStartupInfo;
			PROCESS_INFORMATION lpProcessInfo;
			memset(&lpStartupInfo, 0, sizeof(lpStartupInfo));
			memset(&lpProcessInfo, 0, sizeof(lpProcessInfo));

			/* Create the process */
			if (!CreateProcessA(exePath.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, startDirectory.c_str(), &lpStartupInfo, &lpProcessInfo))
			{
				std::cout << "Not Worknig" << GetLastError() << std::endl;
			}
			break;
	}

	return TRUE;
}