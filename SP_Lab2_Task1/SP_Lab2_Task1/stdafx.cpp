// stdafx.cpp : source file that includes just the standard includes
// SP_Lab2_Task1.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

void PrintError(LPCTSTR error_text)
{
	_tprintf(_T("[ERROR]\t%s\n"), error_text);
}

void PrintWin32Error(LPCTSTR sender)
{
	LPVOID lpMsg = nullptr;
	DWORD dwError = GetLastError();
	// Formating message.
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsg,
		0,
		NULL);
	_tprintf(_T("[ERROR] %s > %s"), sender, lpMsg);
	LocalFree(lpMsg);
}