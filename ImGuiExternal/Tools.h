#pragma once
#include<Windows.h>
#include<string>
#include<TlHelp32.h>

HANDLE sexroblox;
void setupchitchit( HANDLE concac)
{
	sexroblox = concac;
}
template <typename T>
T ReadMemoryEx(DWORD BaseAddress)
{

	T Buffer;
	ReadProcessMemory(sexroblox, (LPCVOID)BaseAddress, &Buffer, sizeof(Buffer), nullptr);

	return Buffer;
}

INT GetTrueProcessId(const char* ProcessName);
