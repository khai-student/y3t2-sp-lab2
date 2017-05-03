#include "stdafx.h"
#include "ClassRecord.h"

Record::Record()
{
	// Clearing all.
	IsDeleted = false;
	id = 0;
	modification_counter = 0;
	memset(text, 0, sizeof(text));

	SYSTEMTIME system_time = { 0 };
	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &ctime);
}

Record::Record(const DWORD& id) : Record()
{
	this->id = id;
	// Getting current time.
	UpdateCreationTime();
}

Record::Record(const DWORD& id, const FILETIME& ctime, const TCHAR* text, const DWORD text_length, const DWORD& m_counter) : Record()
{
	this->id = id;
	this->ctime = ctime;
	this->modification_counter = m_counter;
	SetText(text, text_length);
	// WARN - you dont handle SetText errors.
}

Record::~Record()
{
}

DWORD Record::GetRecordSize()
{
	return ((DWORD)sizeof(id) + sizeof(ctime) + sizeof(modification_counter) + GetTextArraySize()) + 1; // 1 - is deleted flag size
}

HRESULT Record::SetId(const DWORD& id)
{
	HRESULT result_status = S_OK;
	this->id = id;
	return result_status;
}

DWORD Record::GetId() const
{
	return this->id;
}

BOOL Record::UpdateCreationTime()
{
	SYSTEMTIME system_time = { 0 };
	GetSystemTime(&system_time);
	return SystemTimeToFileTime(&system_time, &ctime);
}

FILETIME Record::GetCreationTime() const
{
	return this->ctime;
}

HRESULT Record::SetText(const TCHAR * new_text, const DWORD length)
{
	HRESULT result_status = S_OK;

	if (length > GetTextArrayLength()) {
		result_status = E_OUTOFMEMORY;
	}
	else {
		memcpy(text, new_text, length * sizeof(new_text[0]));
	}

	return result_status;
}

HRESULT Record::GetText(TCHAR* text, const DWORD buffer_length) const
{
	HRESULT result_status = S_OK;

	if (buffer_length >= GetTextArraySize())
	{
		memcpy(text, this->text, GetTextArraySize());
	}
	else
	{
		result_status = ERROR;
	}

	return result_status;
}

DWORD Record::GetTextArrayLength()
{
	return RECORD_TEXT_SIZE_LIMIT;
}

DWORD Record::GetTextArraySize()
{
	return RECORD_TEXT_SIZE_LIMIT * sizeof(TCHAR);
}

void Record::ResetModificationCounter()
{
	modification_counter = 0;
}

DWORD Record::GetModificationCounter() const
{
	return this->modification_counter;
}

HRESULT Record::Dumps(BYTE* buffer, const DWORD& buffer_size) const
{
	HRESULT result_status = S_OK;
	// Checking space enough for serialization.
	if (buffer_size < GetRecordSize())
	{
		result_status = E_OUTOFMEMORY;
	}
	else
	{	// Filling buffer with raw data.
		DWORD offset = 0;
		if (IsDeleted == true)
		{
			buffer[offset] = (BYTE)0x01;
		}
		else
		{
			buffer[offset] = (BYTE)0x00;
		}
		offset += sizeof(BYTE);
		memcpy(buffer + offset, &id, sizeof(id));
		offset += sizeof(id);
		memcpy(buffer + offset, &ctime, sizeof(ctime));
		offset += sizeof(ctime);
		memcpy(buffer + offset, &modification_counter, sizeof(modification_counter));
		offset += sizeof(modification_counter);
		memcpy(buffer + offset, text, GetTextArraySize());
	}
	return result_status;
}

HRESULT Record::Loads(const BYTE* buffer, const DWORD& data_length)
{
	HRESULT result_status = S_OK;
	// Checking data_length to be the same size as we need.
	if (data_length != GetRecordSize())
	{
		result_status = E_INVALIDARG;
	}
	else
	{	// Filling this object with data from buffer.
		DWORD offset = 0; // pass is_deleted flag
		if (buffer[offset] == 0x00)
		{
			IsDeleted = false;
		}
		else
		{
			IsDeleted = true;
		}
		offset += sizeof(BYTE);
		memcpy(&id, buffer + offset, sizeof(id));
		offset += sizeof(id);
		memcpy(&ctime, buffer + offset, sizeof(ctime));
		offset += sizeof(ctime);
		memcpy(&modification_counter, buffer + offset, sizeof(modification_counter));
		offset += sizeof(modification_counter);
		memcpy(text, buffer + offset, sizeof(text));
	}
	return result_status;
}
