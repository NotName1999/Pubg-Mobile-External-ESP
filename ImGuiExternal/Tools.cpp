#include <windows.h>
#include<iostream>
#include<TlHelp32.h>


INT GetTrueProcessId(const char* ProcessName)
{
	INT ProcessId = 0;
	INT ThreadCount = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	PROCESSENTRY32 ProcessEntry32;
	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &ProcessEntry32))
	{
		do
		{
			if (_stricmp(ProcessEntry32.szExeFile, ProcessName) == 0 &&
				ProcessEntry32.cntThreads > ThreadCount)
			{
				ThreadCount = ProcessEntry32.cntThreads;
				ProcessId = (INT)ProcessEntry32.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &ProcessEntry32));
	}

	CloseHandle(hSnapshot);
	return ProcessId;
}

