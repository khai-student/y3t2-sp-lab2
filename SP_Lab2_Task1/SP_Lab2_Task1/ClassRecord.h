#ifndef _CLASSRECORD
#define _CLASSRECORD

#include "stdafx.h"

#define RECORD_TEXT_SIZE_LIMIT 16

class Record
{
public:
	Record();

	// Used for creation of new record.
	Record(const DWORD& id);
	// Used for creation record from existing one.
	Record(const DWORD& id, const FILETIME& ctime, const TCHAR* text, const DWORD text_length, const DWORD& m_counter = 0);
	~Record();

	static DWORD GetRecordSize();
	static DWORD GetTextArrayLength();
	static DWORD GetTextArraySize();

	HRESULT SetId(const DWORD& id);
	DWORD GetId() const;

	HRESULT SetText(const TCHAR* new_text, const DWORD length);
	HRESULT GetText(TCHAR* text, const DWORD buffer_length) const;

	/**
		\brief	Update ctime to current time.
	*/
	BOOL UpdateCreationTime();
	FILETIME GetCreationTime() const;

	void ResetModificationCounter();
	DWORD GetModificationCounter() const;

	BOOL IsDeleted;
	HRESULT Dumps(BYTE* buffer, const DWORD& buffer_size) const;
	HRESULT Loads(const BYTE* buffer, const DWORD& data_length);

private:
	DWORD id;
	FILETIME ctime;
	TCHAR text[RECORD_TEXT_SIZE_LIMIT];
	DWORD modification_counter;
};

#endif // !_CLASSRECORD