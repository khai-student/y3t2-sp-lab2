// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <cstdio>
#include <tchar.h>
#include <windows.h>
#include <string>
#include <cstdlib>
#include <vector>

// TODO: reference additional headers your program requires here

/**
\brief	Prints error_text with preffix [ERROR] to stdout.
\param[in]	error_text	Text to print.
*/
void PrintError(LPCTSTR error_text);

/**
\brief	Gets last Win32 error and prints it to stdout.
\param[in]	sender	String represents part of the program, where exception has been handled.
*/
void PrintWin32Error(LPCTSTR sender);

#ifdef UNICODE
#define	_tsscanf swscanf
#define	_tsprintf swprintf
#else
#define	_tsscanf sscanf
#define	_tsprintf sprintf
#endif