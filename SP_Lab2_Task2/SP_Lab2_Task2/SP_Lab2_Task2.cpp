// SP_Lab2_Task2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleMenu.h"
#define FILENAME_MAX_LENGTH MAX_PATH - 1

TCHAR* filename = (TCHAR*)LocalAlloc(LPTR, MAX_PATH);

void ReadFilename();

void ls();
void mkdir();
void cp();
void mv();
void rm();
void stat();


int _tmain()
{
	// Creating menu.
	Menu menu(_T("SP_Lab2_Task1 Menu"));
	menu.ItemAdd(new MenuItem(_T("List files/dirs."), ls));
	menu.ItemAdd(new MenuItem(_T("Make directory."), mkdir));
	menu.ItemAdd(new MenuItem(_T("Copy file/dir."), cp));
	menu.ItemAdd(new MenuItem(_T("Move file/dir."), mv));
	menu.ItemAdd(new MenuItem(_T("Delete file/dir."), rm));
	menu.ItemAdd(new MenuItem(_T("Get info about file/dir."), stat));

	// Activating menu.
	while (menu.Show() != E_ABORT)
	{
		system("pause && clear");
	}

	LocalFree(filename);
    return 0;
}

void ReadFilename()
{
	_tprintf(_T("Input file/dir name: "));

	TCHAR ch = _T('0');
	do
	{
		_tscanf(_T("%c"), &ch);
	} while (ch == _T('\n') || ch == _T(' '));
	for (SIZE_T char_index = 0; char_index < MAX_PATH - 1 && ch != _T('\n'); ++char_index)
	{
		filename[char_index] = ch;
		_tscanf(_T("%c"), &ch);
	}
}

void ls()
{
	WIN32_FIND_DATA ffd = { 0 };
	LARGE_INTEGER filesize = { 0 };

	HANDLE hFile = FindFirstFile(_T(".\\*"), &ffd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("FindFirstFile failed (%d)\n", GetLastError());
		return;
	}
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			_tprintf(_T("  %-40s   <DIR>\n"), ffd.cFileName);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			_tprintf(_T("  %-40s   %I64d bytes\n"), ffd.cFileName, filesize.QuadPart);
		}
	} while (FindNextFile(hFile, &ffd) != 0);


	FindClose(hFile);
}

void mkdir()
{
	ReadFilename();
	if (!CreateDirectory(filename, NULL))
	{
		_tprintf(_T("Cannot create directory with asked filename."));
	}
}

void cp()
{
}

void mv()
{
}

void rm()
{
	ReadFilename();

	WIN32_FIND_DATA ffd = { 0 };

	HANDLE hFile = FindFirstFile(filename, &ffd);

	if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (!RemoveDirectory(filename))
		{
			_tprintf(_T("Cannot remove directory with asked name."));
		}
	}
	else
	{
		if (!DeleteFile(filename))
		{
			_tprintf(_T("Cannot remove file with asked name."));
		}
	}
}

void stat()
{
}
