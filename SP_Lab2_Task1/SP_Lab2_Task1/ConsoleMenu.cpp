#include "stdafx.h"
#include "ConsoleMenu.h"

MenuItem::MenuItem()
{
	this->text = (LPTSTR)malloc(DEFAULT_TEXT_LENGTH * sizeof(TCHAR));
	CallbackFunction = nullptr;
}

MenuItem::MenuItem(const TCHAR* text, void(DatabaseController::*CallbackFunction)())
{
	// Applying text.
	this->text = (LPTSTR)malloc((_tcslen(text)+1) * sizeof(TCHAR));
	_tcscpy(this->text, text);
	// Setting callback.
	this->CallbackFunction = CallbackFunction;
}

MenuItem::~MenuItem()
{
	free(text);
}

LPTSTR MenuItem::GetText()
{
	return this->text;
}

Menu::Menu()
{
	// Default header setting.
	header = (LPTSTR)malloc(DEFAULT_TEXT_LENGTH * sizeof(TCHAR));
	db_controller = nullptr;
	_tcscpy(header, _T("Console Menu"));
	// Makeing vector.
	items = new std::vector<MenuItem*>();
}

Menu::Menu(LPCTSTR header) : Menu()
{
	SetHeader(header);
}

Menu::~Menu()
{
	free(header);
	items->clear();
	delete items;
}

HRESULT Menu::SetHeader(LPCTSTR header)
{
	HRESULT result_status = S_OK;
	
	UINT32 new_header_length = _tcslen(header);

	if (new_header_length == 0)
	{	// if empty header string
		result_status = E_FAIL;
	}
	else if (_tcslen(this->header) != new_header_length)
	{	// reallocating 
		if (this->header == nullptr)
		{
			this->header = (LPTSTR)malloc((new_header_length + 1) * sizeof(TCHAR));
		}
		else
		{
			this->header = (LPTSTR)realloc(this->header, (new_header_length + 1) * sizeof(TCHAR));
		}
		
		if (this->header == NULL)
		{
			result_status = E_OUTOFMEMORY;
		}
		this->header[new_header_length] = _T('\0');
	}
	
	if (result_status == S_OK)
	{
		_tcscpy(this->header, header);
	}

	return result_status;
}

HRESULT Menu::SetDatabaseController(DatabaseController * db_controller)
{
	HRESULT result_status = S_OK;

	if (db_controller == nullptr)
	{
		result_status = E_POINTER;
	}
	else
	{
		this->db_controller = db_controller;
	}
	
	return result_status;
}

HRESULT Menu::Show()
{

	HRESULT result_status = S_OK;
	
	UINT16 items_count = this->items->size();
	// check if there is no items
	if (!items_count)
	{
		_tprintf(_T("No items in menu. Add some first.\n"));
		result_status = E_FAIL;
	}
	else
	{
		// print header
		_tprintf(_T("\n\n\t< %s >\n"), this->header);
		// printing menu items
		for (size_t item_index = 0; item_index < items_count; item_index++)
		{	// for each item
			_tprintf(_T("%i. %s\n"), item_index + 1, this->items->at(item_index)->GetText());
		}
		_tprintf(_T("%i. Exit\n\n"), items_count + 1);
		// asking choose
		_tprintf(_T("Input menu item number: "));
		UINT16 choosen_index = items_count + 2;
		TCHAR ch = _T('0');
		do
		{
			_tscanf(_T("%c"), &ch);
		} while ((ch < _T('0') || ch > _T('9')));
		choosen_index = ch - _T('0');
		// checking
		if (--choosen_index >= 0 && choosen_index < items_count)
		{	// executing callback.
			(db_controller->*(this->items->at(choosen_index)->CallbackFunction))();
		}
		else if (choosen_index == items_count)
		{	// then exit has been chosen
			result_status = E_ABORT;
		}
		else
		{
			// good bye msg
			_tprintf(_T("You have entered wrong data.\n"));
			result_status = NTE_BAD_DATA;
		}
	}
	return result_status;
}

HRESULT Menu::ItemAdd(MenuItem* new_item)
{
	HRESULT result_status = S_OK;

	this->items->push_back(new_item);

	return result_status;
}