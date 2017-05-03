#ifndef _CONSOLE_MENU
#define _CONSOLE_MENU

#include 

#define __MS_VISUAL_STUDIO

class Menu
{
public:
	Menu();
	~Menu();

	void Menu_SetHeader(const char* header);
	void Menu_Activate(const Menu& menu, void(**result)());
	void Menu_ItemAdd(Menu& menu, const char* text, void(*callback)());
	void Menu_Destroy(Menu& menu);

private:
	char* header;
	MenuItem* items;
	uint8_t items_count;
};

Menu::Menu()
{
}

Menu::~Menu()
{
}

typedef struct {
	char* text;
	void(*function)();
} MenuItem;
#endif // !_CONSOLE_MENU