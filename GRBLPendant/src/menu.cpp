//=========================================================
//Project: GRBL Pendant
//Module:  menu.cpp
//=========================================================
//
// GRBLPendant CNC control Copyright(C) 2021 Andrew Fernie
//
// This program is free software : you can redistribute it and /
// or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//=========================================================
//
// This project includes code from the  XLCD/SerialSpy
// project by Frank Herrmann.
// https://github.com/xpix/XLCD/tree/master/serialspy
//
//=========================================================

#include "menu.h"
#include "config.h"

Menu::Menu(MenuParameterItem pMenuParameters[], int numItems, LiquidCrystal_I2C* panel, long *pEncoderValue)
{
	pParameters = pMenuParameters;
	pMenuPanel = panel;
	pEncoderPosition = pEncoderValue;
	m_NumMenuItems = numItems;
}


void Menu::Draw()
{
	uint8_t         i;
	char            valueBuf[15];
	char			lineBuf[LCDCols];
	float           floatVal;
	int             intVal;
	uint8_t         uint8Val;
	int				paramIndex;

	long             menuOffset;

	menuOffset = (*pEncoderPosition);

	menuOffset = max(0, min(menuOffset, m_NumMenuItems - LCDRows));



	for (i = 0; i < LCDRows; i++) {

		paramIndex = i + menuOffset;
		pMenuPanel->setCursor(0, i);
		pMenuPanel->print(LCDEmpty);  // erase the line
		pMenuPanel->setCursor(0, i);

		if (paramIndex == *pEncoderPosition)
			strcpy(lineBuf, ">");
		else
		    strcpy(lineBuf, " ");


		strncat(lineBuf, ((*(pParameters + paramIndex)).name), MenuTextLength);

		switch (getType(paramIndex)) {
		case MenuParameterType::ParamStatic:
			pMenuPanel->print(lineBuf);
			break;

		case MenuParameterType::ParamFloat:
			floatVal = *(float*)((*(pParameters + paramIndex)).variable);
			printFloat(valueBuf, floatVal, getScale(paramIndex));
			strcat(lineBuf, " ");
			strcat(lineBuf, valueBuf);
			pMenuPanel->print(lineBuf);
			break;

		case MenuParameterType::ParamInt:
			intVal = *(int*)((*(pParameters + paramIndex)).variable);
			sprintf(valueBuf, "%d", intVal);
			strcat(lineBuf, " ");
			strcat(lineBuf, valueBuf);
			pMenuPanel->print(lineBuf);
			break;

		case MenuParameterType::ParamOnOff:
			uint8Val = *(uint8_t*)((*(pParameters + paramIndex)).variable);

			if (uint8Val == 0) 
			{
				strcat(lineBuf, " Off");
			}
			else 
			{
				strcat(lineBuf, " On");
			}

			pMenuPanel->print(lineBuf);

			break;

		case MenuParameterType::ParamYesNo:
			uint8Val = *(uint8_t*)((*(pParameters + paramIndex)).variable);

			if (uint8Val == 0)
			{
				strcat(lineBuf, " No");
			}
			else
			{
				strcat(lineBuf, " Yes");
			}

			pMenuPanel->print(lineBuf);

			break;

		case MenuParameterType::ParamInMm:
			uint8Val = *(uint8_t*)((*(pParameters + paramIndex)).variable);

			if (uint8Val == 0)
			{
				strcat(lineBuf, " In");
			}
			else
			{
				strcat(lineBuf, " mm");
			}

			pMenuPanel->print(lineBuf);

			break;

		default:
			break;
		}
	}
}


uint8_t		Menu::getType(uint8_t paramNum)
{
	return  (*(pParameters + paramNum)).type;
};

float		Menu::getMin(uint8_t paramNum)
{
	return 	(*(pParameters + paramNum)).min;
};

float		Menu::getMax(uint8_t paramNum)
{
	return 	(*(pParameters + paramNum)).max;
};

uint8_t		Menu::getScale(uint8_t paramNum)
{
	return 	(*(pParameters + paramNum)).scale;

};

void		Menu::getName(uint8_t paramNum, char* nameReturn)
{
	char c;
	uint8_t i = 0;

	do {
		c = (*(pParameters + paramNum)).name[i];
		nameReturn[i++] = c;
	} while (c != 0 && i < MenuTextLength);
};

uint8_t Menu::getIndexFromName(char* nameIn)
{
	uint8_t i;
	char tmpStr[MenuTextLength];

	for (i = 0; i < LCDRows; i++) {
		getName(i, tmpStr);
		if (strncmp(tmpStr, nameIn, MenuTextLength) == 0) {
			break;
		}
	}

	if (i < LCDRows) {
		return i;
	}
	else {
		return 255;    // not found
	}
}

void Menu::setValueFloat(uint8_t index, float value)
{
	*(float*)((*(pParameters + index)).variable) = value;
}

uint8_t Menu::setValueFloatFromName(char* nameIn, float valueIn)
{
	uint8_t	index;
	index = getIndexFromName(nameIn);
	if (index != 255) {
		setValueFloat(index, valueIn);
	}

	return index;
}

void Menu::printFloat(char* outBuffer, float val, byte precision)
{
	// prints val with number of decimal places determine by precision
	// precision is a number from 0 to 6 indicating the desired decimial places
	// example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)
	char	tmpBuf[10];

	sprintf(outBuffer, "%d", int(val));  //prints the int part
	if (precision > 0) {
		strcat(outBuffer, "."); // print the decimal point
		unsigned int frac;
		unsigned int mult = 1;
		byte padding = precision - 1;
		while (precision--) {
			mult *= 10;
		}

		if (val >= 0) {
			frac = (val - int(val)) * mult;
		}
		else {
			frac = (int(val) - val) * mult;
		}
		unsigned long frac1 = frac;
		while (frac1 /= 10) {
			padding--;
		}
		while (padding--) {
			strcat(outBuffer, "0");
		}
		sprintf(tmpBuf, "%d", frac);
		strcat(outBuffer, tmpBuf);
	}
}

int Menu::GetNumMenuItems()
{
	return m_NumMenuItems;
}

void Menu::EditItem(int itemIndex)
{
	pMenuPanel->clear();

	// Edit the selected item

	Draw();				// Cleanup the screen
};

int	Menu::GetCurrentItemIndex()
{

	return false;
}
