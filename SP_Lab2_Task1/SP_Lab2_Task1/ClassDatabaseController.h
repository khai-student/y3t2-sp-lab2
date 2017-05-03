#ifndef _DATABASEOPERATIONS
#define _DATABASEOPERATIONS

#include "stdafx.h"
#include "ClassRecord.h"

class DatabaseController
{
public:
	DatabaseController();
	DatabaseController(HANDLE hFileDb, DWORD records_limit);
	~DatabaseController();

	void SelectAll();
	void SelectRecord();
	void AlterRecord();
	void InsertRecord();
	void DropRecord();
	void DropAll();

private:
	HANDLE hFileDb;	// handle to opened DB file
	DWORD RecordsLimit;		// limit qty for records in file
	DWORD RecordsWritten;	// qty of records in file

	HRESULT ReadDbInfo();

	HRESULT UpdateDbInfo();

	HRESULT FindRecord(const DWORD sought_id, LPDWORD record_offset = 0, Record** found_record = nullptr) const;

	HRESULT AskForRecordId(LPDWORD result) const;

	HRESULT PrintRecordContent(Record* record) const;

	HRESULT WriteRecord(const DWORD offset, const Record* record);

	HRESULT FindDeletedRecord(Record** record, LPDWORD offset = nullptr) const;

	void ReadText(TCHAR* buffer, DWORD buffer_length);
};

#endif // !_DATABASEOPERATIONS
