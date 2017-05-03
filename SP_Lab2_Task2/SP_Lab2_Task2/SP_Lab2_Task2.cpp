// SP_Lab2_Task2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleMenu.h"
#define FILENAME_MAX_LENGTH 255
#define COMMAND_MAX_LENGTH FILENAME_MAX_LENGTH + 25

HRESULT ComposeCommand(const TCHAR* cmd, const DWORD sizeof_cmd, TCHAR result[COMMAND_MAX_LENGTH]);
HRESULT ReadLine(const TCHAR* asking_msg, TCHAR* result, const DWORD result_buffer_size);

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
	menu.ItemAdd(new MenuItem(_T("List files/dirs."), mkdir));
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
    return 0;
}

void ls()
{
	system("ls -l");
}

void mkdir()
{
	TCHAR* command = (TCHAR*)LocalAlloc(LPTR, COMMAND_MAX_LENGTH);

	if (command == nullptr)
	{
		_tprintf(_T("Cannot allocate memory."));
		return;
	}

	ComposeCommand(_T("mkdir ./"), _tcslen(_T("mkdir ./")) * sizeof(TCHAR), command);
	_tsystem(command);
	
	LocalFree(command);
}
