// 
// 
// 

#include "menu.h"
#include "config.h"

Menu::Menu(MenuParameterItem pMenuParameters[], LiquidCrystal_I2C* panel, long *pEncoderValue)
{
	pParameters = pMenuParameters;
	pMenuPanel = panel;
	pEncoderPosition = pEncoderValue;
}


void Menu::Draw()
{
	uint8_t         i;
	char            valueBuf[15];
	char			lineBuf[LCD_cols];
	float           floatVal;
	int             intVal;
	uint8_t         uint8Val;
	int				paramIndex;

	long             menuOffset;

	menuOffset = (*pEncoderPosition)/4;
	menuOffset = max(0, min(menuOffset, 7 - LCD_rows));

	for (i = 0; i < LCD_rows; i++) {

		paramIndex = i + menuOffset;
		pMenuPanel->setCursor(0, i);
		pMenuPanel->print(LCD_EMPTY);  // erase the line
		pMenuPanel->setCursor(0, i);

		strncpy(lineBuf, ((*(pParameters + paramIndex)).name), MenuTextLength);

		switch (getType(i)) {
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

	for (i = 0; i < LCD_rows; i++) {
		getName(i, tmpStr);
		if (strncmp(tmpStr, nameIn, MenuTextLength) == 0) {
			break;
		}
	}

	if (i < LCD_rows) {
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