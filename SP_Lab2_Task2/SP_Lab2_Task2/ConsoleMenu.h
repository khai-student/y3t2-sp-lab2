#ifndef _CONSOLE_MENU
#define _CONSOLE_MENU

#include "stdafx.h"

#define DEFAULT_TEXT_LENGTH 64

class MenuItem
{
public:
	MenuItem();
	MenuItem(const TCHAR* text, void (*CallbackFunction)());
	~MenuItem();

	void(*CallbackFunction)();
	LPTSTR GetText();

private:
	LPTSTR text;
};

class Menu
{
public:
	Menu();
	Menu(LPCTSTR header);
	~Menu();

	HRESULT SetHeader(LPCTSTR header);
	HRESULT Show();
	HRESULT ItemAdd(MenuItem* new_item);

private:
	LPTSTR header;
	std::vector<MenuItem*>* items;
};

#endif // !_CONSOLE_MENU