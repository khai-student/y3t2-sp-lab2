// SP_Lab2_Task1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ClassRecord.h"
#include "ConsoleMenu.h"
#include "ClassDatabaseController.h"

/**
	\brief	Prints help message to stdout.
*/
void PrintHelp();

/**
	\brief	Parse cmdline arguments and opens DB file.
	\return	If success - S_OK.
*/
HRESULT ParseCmdLineArgs(UINT8 argc, TCHAR** argv);

TCHAR FileName[128] = { 0 };	// DB filename
DatabaseController *DbController;

int _tmain(UINT8 argc, TCHAR** argv)
{
	int result_status = 0;
	
	if (ParseCmdLineArgs(argc, argv) != S_OK)
	{
		result_status = 1;
	}
	else
	{
		// Creating menu.
		Menu menu(_T("SP_Lab2_Task1 Menu"));
		menu.ItemAdd(new MenuItem(_T("Select record by ID."), &DatabaseController::SelectRecord));
		menu.ItemAdd(new MenuItem(_T("Select all records."), &DatabaseController::SelectAll));
		menu.ItemAdd(new MenuItem(_T("Alter record by ID."), &DatabaseController::AlterRecord));
		menu.ItemAdd(new MenuItem(_T("Insert new record."), &DatabaseController::InsertRecord));
		menu.ItemAdd(new MenuItem(_T("Drop record by ID."), &DatabaseController::DropRecord));
		menu.ItemAdd(new MenuItem(_T("Drop all records."), &DatabaseController::DropAll));

		menu.SetDatabaseController(DbController);

		// Activating menu.
		while (menu.Show() != E_ABORT)
		{
			system("pause && clear");
		}
	}

Exit:
    return result_status;
}

void PrintHelp()
{
	_tprintf(_T("[INFO]\tthis.exe -f file.hex -l 50 \\ file.hex is our DB with 50 records limit.\n"));
}

HRESULT ParseCmdLineArgs(UINT8 argc, TCHAR** argv) {
	HRESULT result_status = S_OK;
	HANDLE hFileDb = nullptr;
	DWORD records_limit = 0;
	// Manage cmd. line args.
	if (argc == 1 || argc == 2 || argc == 4 || argc > 5)
	{
		PrintError(_T("Wrong command line argumets quantity."));
		result_status = E_INVALIDARG;
		goto Exit;
	}
	else if (_tcscmp(_T("-f"), argv[1]) != 0)
	{	// Checking -f key.
		PrintHelp();
		result_status = E_INVALIDARG;
		goto Exit;
	}
	if (argc == 5)
	{	// If the limit is set.
		if (_tcscmp(_T("-l"), argv[3]) != 0)
		{	// Checking key.
			PrintHelp();
			result_status = E_INVALIDARG;
			goto Exit;
		}
		// Trying to read records quantity limits.
		if (_tsscanf(argv[4], _T("%u"), &records_limit) != 1)
		{
			PrintError(_T("Wrong command line argumets quantity."));
			result_status = E_INVALIDARG;
			goto Exit;
		}
	}

	hFileDb = CreateFile(
		argv[2],
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	// Checking an error.
	if (hFileDb == INVALID_HANDLE_VALUE)
	{
		PrintWin32Error(_T("Cannot open file"));
		result_status = E_FAIL;
	}
	else
	{
		DbController = new DatabaseController(hFileDb, records_limit);
	}
	
Exit:
	return result_status;
}