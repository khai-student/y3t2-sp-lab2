#include "stdafx.h"
#include "ClassDatabaseController.h"

DatabaseController::DatabaseController()
{
	this->hFileDb = nullptr;
	this->RecordsLimit = 50;
	this->RecordsWritten = 0;
}

DatabaseController::DatabaseController(HANDLE hFileDb, DWORD records_limit)
{
	this->hFileDb = hFileDb;

	ReadDbInfo();

	if (this->RecordsLimit != records_limit)
	{
		this->RecordsLimit = records_limit;
		UpdateDbInfo();
	}
}

DatabaseController::~DatabaseController()
{
	CloseHandle(this->hFileDb);
}

void DatabaseController::SelectAll()
{
	DWORD records_found = 0;

	for (DWORD asked_id = 0, record_offset = 0; asked_id < RecordsLimit && records_found < RecordsWritten; ++asked_id)
	{
		// Reading the record
		Record* record = nullptr;
		if (FindRecord(asked_id, nullptr, &record) == S_OK)
		{
			if (record->IsDeleted)
			{
				continue;
			}
			++records_found;
			// if reading is successfull
			if (this->PrintRecordContent(record) != S_OK)
			{
				PrintError(_T("Record print failed"));
				delete record;
				break;
			}
			else
			{
				_tprintf(_T("\n"));
			}
			delete record;
		}
	}
}

HRESULT DatabaseController::PrintRecordContent(Record* record) const
{
	HRESULT result_status = S_OK;

	if (record == nullptr)
	{
		result_status = E_POINTER;
	}
	else
	{
		_tprintf(_T("Id:\t%u\n"), record->GetId());
		_tprintf(_T("Created:\t"));

		TCHAR* buffer = nullptr;
		DWORD buffer_length = 0;
		if (Record::GetTextArrayLength() < 256)
		{
			buffer = new TCHAR[256]{ 0 };
			buffer_length = 256;
		}
		else
		{
			buffer_length = Record::GetTextArrayLength();
			buffer = new TCHAR[buffer_length]{ 0 };
		}

		SYSTEMTIME system_time = { 0 };
		FILETIME ctime = record->GetCreationTime();

		FileTimeToLocalFileTime(&ctime, &ctime);
		FileTimeToSystemTime(&ctime, &system_time);
		GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_LONGDATE, &system_time, NULL, buffer, buffer_length);
		_tprintf(_T("%s "), buffer);
		GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &system_time, NULL, buffer, buffer_length);
		_tprintf(_T("%s\n"), buffer);

		_tprintf(_T("Modification counter: %u\n"), record->GetModificationCounter());

		memset(buffer, 0, buffer_length);
		if (record->GetText(buffer, buffer_length) != S_OK)
		{
			result_status = E_ABORT;
		}
		_tprintf(_T("Text:\n%s\n"), buffer);

		delete[] buffer;
	}
	return result_status;
}

void DatabaseController::SelectRecord()
{
	DWORD asked_record = 0;
	if (AskForRecordId(&asked_record) != S_OK)
	{
		PrintError(_T("NaN is written."));
		return;
	}
	Record* record = nullptr;
	FindRecord(asked_record, nullptr, &record);
	if (record == nullptr || record->IsDeleted == true)
	{
		PrintError(_T("Record is not found."));
		return;
	}
	// if success
	if (PrintRecordContent(record) != S_OK)
	{
		PrintError(_T("Printout error."));
	}
	delete record;
}

void DatabaseController::AlterRecord()
{
	DWORD asked_record = 0;
	if (AskForRecordId(&asked_record) != S_OK)
	{
		PrintError(_T("NaN is written."));
		return;
	}
	// reading record form file
	DWORD record_offset = 0;
	Record* record = nullptr;

	if ((FindRecord(asked_record, &record_offset, &record) != S_OK) || record == nullptr)
	{
		PrintError(_T("Record is not found."));
		return;
	}
	// showing record content
	if (PrintRecordContent(record) != S_OK)
	{
		PrintError(_T("Printout error."));
		delete record;
		return;
	}
	// then now we can ask for new text
	TCHAR* text = new TCHAR[Record::GetTextArrayLength()]{ 0 };
	// asking for a new text
	_tprintf(_T("Input text (up to %u chars): "), Record::GetTextArrayLength());
	ReadText(text, Record::GetTextArrayLength());
	// saving it
	if (record->SetText(text, Record::GetTextArrayLength()) != S_OK)
	{
		PrintError(_T("Text is too long."));
		goto Exit;
	}
	// writing record back
	record->UpdateCreationTime();
	if (WriteRecord(record_offset, record) != S_OK)
	{
		PrintError(_T("Cannot write record back."));
	}
Exit:
	if (text != nullptr)
	{
		delete[] text;
	}
	if (record != nullptr)
	{
		delete record;
	}
}

void DatabaseController::InsertRecord()
{
	// check records qty
	if (RecordsWritten >= RecordsLimit)
	{
		PrintError(_T("Records limit is reached."));
		return;
	}
	// find deleted record offset
	DWORD offset = 0;
	Record* record = nullptr;
	BOOL is_deleted_found = false;
	FindDeletedRecord(&record, &offset);
	// checking offset
	if (offset == 0 || record == nullptr)
	{	// then no record was found
		record = new Record(RecordsWritten);
		offset = 2 * sizeof(DWORD) + RecordsWritten*Record::GetRecordSize();
	}
	else
	{
		is_deleted_found = true;
	}
	// fill record
	record->IsDeleted = false;
	if (record->UpdateCreationTime() != true)
	{
		PrintError(_T("Cannot update record time."));
		return;
	}
	// then now we can ask for new text
	DWORD text_length = Record::GetTextArrayLength();
	TCHAR* text = new TCHAR[text_length]{ 0 };
	// asking for a new text
	_tprintf(_T("Input text (up to %u chars): "), text_length);

	ReadText(text, text_length);

	// saving it
	if (record->SetText(text, text_length) != S_OK)
	{
		PrintError(_T("Text is too long."));
		goto Exit;
	}
	// writing record back
	if (WriteRecord(offset, record) != S_OK)
	{
		PrintError(_T("Cannot write record back."));
	}
	// increment this->RecordsWritten
	if (!is_deleted_found)
	{
		++RecordsWritten;
		UpdateDbInfo();
	}

Exit:
	delete record;
	delete[] text;
}

void DatabaseController::DropRecord()
{
	DWORD asked_record = 0;
	if (AskForRecordId(&asked_record) != S_OK)
	{
		PrintError(_T("NaN is written."));
		return;
	}
	// reading record form file
	DWORD record_offset = 0;
	Record* record = nullptr;

	if (FindRecord(asked_record, &record_offset, &record) != S_OK)
	{
		PrintError(_T("Record is not found."));
		return;
	}
	// mark deleted
	record->IsDeleted = true;
	// writing record back
	if (WriteRecord(record_offset, record) != S_OK)
	{
		PrintError(_T("Cannot write record back."));
	}
	// decrement db header
	this->RecordsWritten--;
	if (UpdateDbInfo() != S_OK)
	{
		PrintError(_T("Cannot update DB header."));
	}
Exit:
	if (record != nullptr)
	{
		delete record;
	}
}

void DatabaseController::DropAll()
{
	// Clear head
	this->RecordsWritten = 0;
	if (UpdateDbInfo() != S_OK)
	{
		PrintWin32Error(_T("Drop all error"));
		return;
	}
}

HRESULT DatabaseController::ReadDbInfo()
{
	HRESULT result_status = S_OK;
	// Read first N bytes from file
	BYTE *buffer = new UINT8[Record::GetRecordSize()];
	// Reading the head of the file
	DWORD bytes_read_qty = 0;
	BOOL isSuccess = ReadFile(this->hFileDb, buffer, sizeof(DWORD) * 2, &bytes_read_qty, NULL);
	if (!isSuccess || bytes_read_qty != sizeof(DWORD) * 2)
	{
		PrintWin32Error(_T("Read file error"));
		result_status = E_ACCESSDENIED;
		goto Exit;
	}
	// First DWORD - Written records qty
	memcpy(&(this->RecordsWritten), buffer, sizeof(this->RecordsWritten));
	DWORD previous_records_limit = 0;
	memcpy(&(this->RecordsLimit), buffer + sizeof(this->RecordsWritten), sizeof(this->RecordsLimit));

Exit:
	delete[] buffer;
	return result_status;
}

HRESULT DatabaseController::UpdateDbInfo()
{
	HRESULT result_status = S_OK;

	SetFilePointer(this->hFileDb, 0, 0, FILE_BEGIN);

	DWORD written_bytes_qty = 0;
	BOOL isSuccess = WriteFile(this->hFileDb, &this->RecordsWritten, sizeof(this->RecordsWritten), &written_bytes_qty, NULL);
	if (!isSuccess || written_bytes_qty != sizeof(this->RecordsWritten))
	{
		PrintWin32Error(_T("Cannot update records written."));
		result_status = SLE_ERROR;
		goto Exit;
	}

	written_bytes_qty = 0;
	isSuccess = WriteFile(this->hFileDb, &this->RecordsLimit, sizeof(this->RecordsLimit), &written_bytes_qty, NULL);
	if (!isSuccess || written_bytes_qty != sizeof(this->RecordsLimit))
	{
		PrintWin32Error(_T("Cannot update records limit."));
		result_status = SLE_ERROR;
		goto Exit;
	}
Exit:
	return result_status;
}

HRESULT DatabaseController::FindRecord(const DWORD sought_id, LPDWORD record_offset, Record** found_record) const
{
	HRESULT result_status = E_ACCESSDENIED;

	DWORD record_size = Record::GetRecordSize();
	DWORD bytes_read_qty = 0;
	BYTE *buffer = new BYTE[record_size];
	// Read all records until we get needed
	OVERLAPPED overlaped = { 0 };
	overlaped.Offset = sizeof(DWORD) * 2;
	do {
		bytes_read_qty = 0;
		ReadFile(this->hFileDb, buffer, record_size, &bytes_read_qty, &overlaped);
		if (bytes_read_qty == record_size)
		{	// work with next record from file
			Record *record = new Record();
			if (record->Loads(buffer, record_size) != S_OK)
			{
				result_status = E_ACCESSDENIED;
				if (record_offset != nullptr)
				{
					*record_offset = 0;
				}
				if (found_record != nullptr)
				{
					*found_record = nullptr;
				}
				delete record;
				goto Exit;
			}
			// If we are there, so we read record correctly
			if (record->GetId() == sought_id/* && !record->IsDeleted()*/)
			{
				if (record_offset != nullptr)
				{
					*record_offset = overlaped.Offset;
				}
				if (found_record != nullptr)
				{
					*found_record = record;
				}
				result_status = S_OK;
				goto Exit;
			}
			delete record;
			// Move offset if there was not needed record
			overlaped.Offset += record_size;
			result_status = S_OK;
		}
	} while (bytes_read_qty);

Exit:
	delete[] buffer;

	return result_status;
}

HRESULT DatabaseController::AskForRecordId(LPDWORD id) const
{
	HRESULT result_status = S_OK;
	// Echo promt
	_tprintf(_T("Input record ID: "));
	// Reading
	if (_tscanf(_T("%lu"), id) == 1)
	{
		if (*id > this->RecordsLimit)
		{
			*id = 0;
			result_status = E_ACCESSDENIED;
		}
	}
	else
	{
		result_status = E_ABORT;
	}
	return result_status;
}

HRESULT DatabaseController::WriteRecord(const DWORD offset, const Record* record)
{
	HRESULT result_status = S_OK;

	DWORD buffer_size = Record::GetRecordSize();
	BYTE* buffer = (BYTE*)malloc(buffer_size * sizeof(BYTE));
	// serializing record
	if (record->Dumps(buffer, buffer_size) != S_OK)
	{
		PrintError(_T("Cannot serialize the record to buffer."));
		result_status = E_ABORT;
		goto Exit;
	}

	DWORD written_bytes_qty = 0;
	OVERLAPPED overlaped = { 0 };
	overlaped.Offset = offset;

	BOOL isSuccess = WriteFile(this->hFileDb, buffer, buffer_size * sizeof(buffer[0]), &written_bytes_qty, &overlaped);

	if (!isSuccess || written_bytes_qty != buffer_size)
	{
		PrintWin32Error(_T("Not all content was successfuly writen to file from buffer."));
		result_status = E_ACCESSDENIED;
		goto Exit;
	}

Exit:
	free(buffer);
	return result_status;
}

HRESULT DatabaseController::FindDeletedRecord(Record** found_record, LPDWORD offset) const
{
	HRESULT result_status = S_OK;

	DWORD record_size = Record::GetRecordSize();
	DWORD bytes_read_qty = 0;
	BYTE *buffer = new BYTE[record_size];
	// Read all records until we get needed
	OVERLAPPED overlaped = { 0 };
	overlaped.Offset = sizeof(DWORD) * 2;
	do {
		bytes_read_qty = 0;
		ReadFile(this->hFileDb, buffer, record_size, &bytes_read_qty, &overlaped);
		if (bytes_read_qty > 0 && bytes_read_qty < record_size)
		{	// Then file is corrupted
			result_status = E_FAIL;
			break;
		}
		else if (bytes_read_qty == record_size)
		{	// work with next record from file
			Record *record = new Record();
			if (record->Loads(buffer, record_size) != S_OK)
			{
				result_status = E_ACCESSDENIED;
				*found_record = nullptr;
				if (offset != nullptr)
				{
					*offset = 0;
				}
				goto Exit;
			}
			// If we are there, so we read record correctly
			if (record->IsDeleted == true)
			{
				*found_record = record;
				if (offset != nullptr)
				{
					*offset = overlaped.Offset;
				}
				goto Exit;
			}
			// Move offset if there was not needed record
			overlaped.Offset += record_size;
			delete record;
		}
	} while (bytes_read_qty);

Exit:
	delete[] buffer;

	return result_status;
}

void DatabaseController::ReadText(TCHAR* buffer, DWORD buffer_length)
{
	TCHAR ch = _T('0');
	do
	{
		_tscanf(_T("%c"), &ch);
	} while (ch == _T('\n') || ch == _T(' '));
	for (SIZE_T char_index = 0; char_index < buffer_length - 1 || ch != _T('\n'); ++char_index)
	{
		if (char_index < buffer_length)
		{
			buffer[char_index] = ch;
		}
		_tscanf(_T("%c"), &ch);
	}
}
