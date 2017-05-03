// SP_Lab2_Task2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleMenu.h"
#define FILENAME_MAX_LENGTH MAX_PATH - 1

#ifdef UNICODE
#define _tsprintf swprintf
#else
#define _tsprintf sprintf
#endif

TCHAR* filename = (TCHAR*)LocalAlloc(LPTR, MAX_PATH);

void ReadFilename(const TCHAR* msg = nullptr);
BOOL IsDir();
void FileTimeToString(FILETIME* filetime, TCHAR buffer[256]);

void cd();
void ls();
void mkdir();
void cp();
void rm();
void stat();

DWORD CALLBACK CopyProgressRoutine(
	_In_     LARGE_INTEGER TotalFileSize,
	_In_     LARGE_INTEGER TotalBytesTransferred,
	_In_     LARGE_INTEGER StreamSize,
	_In_     LARGE_INTEGER StreamBytesTransferred,
	_In_     DWORD         dwStreamNumber,
	_In_     DWORD         dwCallbackReason,
	_In_     HANDLE        hSourceFile,
	_In_     HANDLE        hDestinationFile,
	_In_opt_ LPVOID        lpData
);

int _tmain()
{
	// Creating menu.
	Menu menu(_T("SP_Lab2_Task1 Menu"));
	menu.ItemAdd(new MenuItem(_T("Change directory"), cd));
	menu.ItemAdd(new MenuItem(_T("List files/dirs."), ls));
	menu.ItemAdd(new MenuItem(_T("Make directory."), mkdir));
	menu.ItemAdd(new MenuItem(_T("Copy file/dir."), cp));
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

void ReadFilename(const TCHAR* msg)
{
	memset(filename, 0, MAX_PATH);

	if (msg != nullptr)
	{
		_tprintf(msg);
	}
	else
	{
		_tprintf(_T("Input file/dir name: "));
	}
	

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
/*
	TCHAR ch = _T('0');
	do
	{
		_tscanf(_T("%c"), &ch);
	} while (ch == _T('\n') || ch == _T(' ') || ch == _T('/') || ch == _T('\\'));
	for (SIZE_T char_index = 0; char_index < MAX_PATH - 1 && ch != _T('\n'); ++char_index)
	{
		filename[char_index] = ch;
		do
		{
			_tscanf(_T("%c"), &ch);
		} while (ch == _T('/') || ch == _T('\\'));
	}*/
}

BOOL IsDir()
{
	WIN32_FIND_DATA ffd = { 0 };
	HANDLE hFile = FindFirstFile(filename, &ffd);
	FindClose(hFile);
	return (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 ? FALSE : TRUE;
}

void FileTimeToString(FILETIME* filetime, TCHAR result[256])
{
	memset(result, 0, 256);
	TCHAR buffer[256] = {0};

	SYSTEMTIME system_time = { 0 };

	FileTimeToLocalFileTime(filetime, filetime);
	FileTimeToSystemTime(filetime, &system_time);
	GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_LONGDATE, &system_time, NULL, buffer, 256);
	_tsprintf(result, _T("%s"), buffer);

	result[_tcslen(result)] = ' ';
	memset(buffer, 0, 256);
	GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &system_time, NULL, buffer, 256);
	_tsprintf(result + _tcslen(result), _T("%s"), buffer);
}

void cd()
{
	ReadFilename();
	if (IsDir())
	{
		SetCurrentDirectory(filename);
	}
	else
	{
		_tprintf(_T("Cannot find directory with such name.\n"));
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
	// remembering source filename
	TCHAR* source = (TCHAR*)LocalAlloc(LPTR, MAX_PATH);
	ReadFilename(_T("Input source filename: "));
	memcpy(source, filename, MAX_PATH);
	// getting result filename
	ReadFilename(_T("Input destination filename: "));

	BOOL is_finished = false;
	CopyFileEx(source, filename, CopyProgressRoutine, &is_finished, FALSE, NULL);
	while (!is_finished);

	LocalFree(source);
}

void rm()
{
	ReadFilename();

	if (IsDir())
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
	ReadFilename();
	if (!IsDir())
	{
		HANDLE hFile = CreateFile(
			filename,
			GENERIC_READ,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		// Checking an error.
		if (hFile == INVALID_HANDLE_VALUE)
		{
			_tprintf(_T("Cannot open file"));
			return;
		}

		BY_HANDLE_FILE_INFORMATION info = { 0 };
		if (!GetFileInformationByHandle(hFile, &info))
		{
			_tprintf(_T("Cannot get file information.\n"));
			return;
		}

		TCHAR* buffer = new TCHAR[256]{ 0 };
		_tprintf(_T("Name: %-40s\n"), filename);
		LARGE_INTEGER size = { 0 };
		size.HighPart = info.nFileSizeHigh;
		size.LowPart = info.nFileSizeLow;
		_tprintf(_T("Size: %I64d B\n"), size.QuadPart);
		FileTimeToString(&info.ftCreationTime, buffer);
		_tprintf(_T("Creation time: %s\n"), buffer);
		FileTimeToString(&info.ftLastWriteTime, buffer);
		_tprintf(_T("Write time: %s\n"), buffer);
		FileTimeToString(&info.ftLastAccessTime, buffer);
		_tprintf(_T("Access time: %s\n"), buffer);

		delete[] buffer;
		CloseHandle(hFile);
	}
	else
	{
		_tprintf(_T("Cannot get stat info about dir.\n"));
	}
}

DWORD CALLBACK CopyProgressRoutine(
	_In_     LARGE_INTEGER TotalFileSize,
	_In_     LARGE_INTEGER TotalBytesTransferred,
	_In_     LARGE_INTEGER StreamSize,
	_In_     LARGE_INTEGER StreamBytesTransferred,
	_In_     DWORD         dwStreamNumber,
	_In_     DWORD         dwCallbackReason,
	_In_     HANDLE        hSourceFile,
	_In_     HANDLE        hDestinationFile,
	_In_opt_ LPVOID        lpData
) {
	switch (dwCallbackReason)
	{
	case CALLBACK_CHUNK_FINISHED:
		if (TotalBytesTransferred.QuadPart >= TotalFileSize.QuadPart || TotalFileSize.QuadPart == 0)
		{
			_tprintf(_T("\rOperation progress\t100%%\n"));
			*((BOOL*)lpData) = TRUE;
		}
		else
		{
			_tprintf(_T("\rOperation progress\t%ld%%"), TotalBytesTransferred.QuadPart / TotalFileSize.QuadPart);
		}
		break;
	default:
		break;
	}

	return PROGRESS_CONTINUE;
}
