//=========================================================
//Project: GRBL Pendant
//Module:  LCDMenu.cpp
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------


#include "Arduino.h"
#include "LCDMenu.h"

// symbol to describe selected point
int lcd_c =	0;
int lcd_r =	0;

const char LCD_select_char = '>';
int LCD_offset =	-1;
int LCD_select =	0;

// callback onchange
void (*onchange_cb)(int);

// count entrys
int menucounter = 0;

// default constructor
LCDMenu::LCDMenu(void *l, int	c, int r)
{
	lcd_c =	c;
	lcd_r =	r;
	
	lcd=(MY_LCD*)l;
	lcd->clear();	
} //LCDMenu

void LCDMenu::reinit()
{
	LCD_offset =	-1;
	LCD_select =	0;
	menucounter= 	0;
}

// add menu entry [menutext, callback]
void LCDMenu::add_item(const __FlashStringHelper *ifsh, void (*menu_callback)(int), int callback_param)
{
	strcpy_P(buffer, (char*)pgm_read_dword(&(ifsh)));
	add_item(buffer, menu_callback, callback_param);
}


// add menu entry [menutext, callback]
void LCDMenu::add_item(const char* text, void (*menu_callback)(int), int callback_param)
{
	int i = menucounter++;
	struct menuitem *item = &items[i];
	strncpy(item->name, text, sizeof(item->name));
	if(menu_callback){
		item->callback = menu_callback;
		item->callback_param = callback_param;
	}
}

// display menu at offset
void LCDMenu::display(int offset) {

	// roll over to first entry
	if(LCD_offset >= menucounter)
		LCD_offset = 0;
	if(LCD_offset < 0)
		LCD_offset = menucounter;
	if(LCD_offset >= (menucounter - lcd_r))
		LCD_offset = (menucounter - lcd_r);

	if(LCD_select >= menucounter){
		LCD_offset = 0;
		LCD_select = 0;
	}
	if(LCD_select < 0){
		LCD_select = menucounter - 1;
	}
	
	struct menuitem *item;

	lcd->clear();

	// mexit menu structure
	int pos = 0;
	for (int i = LCD_offset; i <= menucounter; i++)
	{
		if(pos >= lcd_r){
			break;
		}
		// mexit a '-' for selected point
		lcd->setCursor(0, pos);
		lcd->print((i == LCD_select ? LCD_select_char : ' '));

		// mexit menu text
		item = &items[i];
		lcd->setCursor(1, pos);
		lcd->print(item->name);
		
		pos++;
	}
};

void LCDMenu::up()
{
	LCD_select++;
   display(LCD_offset++); // one up
	if(onchange_cb)
		onchange_cb(+1);
}

void LCDMenu::down()
{
	LCD_select--;
	display(LCD_offset--); // one down
	if(onchange_cb)
		onchange_cb(-1);
}

// register onchange event If menu changed
// second argument is state
// 0 = exit; -1 = down; +1 = up
void LCDMenu::onchange(void (*callback)(int)) 
{
	onchange_cb = callback;		
}


// Call menu point
void LCDMenu::mselect() {

	struct menuitem *item;
	item = &items[LCD_select];
	if(item->callback)
		item->callback( item->callback_param );
}

void LCDMenu::about(String text)
{
	LCD_offset = -1;
	lcd->clear();
	lcd->print(text);

	return;
}


void LCDMenu::mexit()
{
	LCD_offset = -1;
	lcd->clear();
	if(onchange_cb)
		onchange_cb(0);
	
	return;
}
