/*
 * module_text.c
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#include "module_text.h"
#include "../meny.h"


uint8_t add_lineMenu_text(config_meny *config, char* name1, char* name2, uint8_t* value, char **textList, uint8_t size, void* postFunc)
{
	if( *value > size) *value = size;

   // Выделяем память для массива строк
	char **textList_ptr = (char **)malloc(size * sizeof(char *));
	if (textList_ptr == NULL) {
		return 1;
	}
	// Выделение памяти под каждую строку и присваивание значения
	for (uint8_t i = 0; i < size; i++)
	{
		textList_ptr[i] = (char *)malloc(MAX_SYMBOL_BUF * sizeof(char));    // например, строки длиной 20 символов
		if(textList_ptr[i] == NULL)
			return 1; // ошибка выделения памяти
        // Проверка на переполнение строки
        if (strlen(textList[i]) >= MAX_SYMBOL_BUF) {
            return 1; // ошибка переполнения буфера
        }
		strcpy(textList_ptr[i], textList[i]);
	}

	// Выделяем память для переменной int16_t
	uint8_t *ptr_size_list = (uint8_t *)malloc(sizeof(uint8_t));
	if (ptr_size_list == NULL)
		return 1; // ошибка выделения памяти

	*ptr_size_list = size - 1;

	strcpy(config->lineMeny[config->sizeLine].name_1, name1);
	strcpy(config->lineMeny[config->sizeLine].name_2, name2);
	config->lineMeny[config->sizeLine].value = value;
	config->lineMeny[config->sizeLine].minValue = 0;
	config->lineMeny[config->sizeLine].maxValue = ptr_size_list;
	config->lineMeny[config->sizeLine].post_func = postFunc;
	config->lineMeny[config->sizeLine].typeValue = t_text_list;
	config->lineMeny[config->sizeLine].list_param = textList_ptr;
	config->sizeLine++;
	return 0; // ok
}

// поверхностное меню настройки текстового списка параметра
void top_meny_changeListText(const config_meny *conf_meny, uint8_t numline, const FontDef_t * font)
{
	if(conf_meny->lineMeny[numline].typeValue != t_text_list) return;

	int8_t   lineBacklight = 0; // номер подсвеченной строки
	uint8_t  offset = 0;        // смещение экрана, смещения видимых строк относительно скрытых, всего масива строк
	uint8_t  posline;
	int16_t  encoder = 0;
    uint8_t  sizeLine = *(uint8_t*)conf_meny->lineMeny[numline].maxValue; // количество строк
	uint8_t  sizeShow_line = 2; // максимальное количество отображаемых строк - 2 это три строки, 0,1,2
	pressButton_TypeDef statusButton = PRESS_NO;

	// узнаем максимальное длинну строк из списка, это нужно для выбора длины подсветки строк
	uint8_t max_strokPixLen = 0;
	for(int i = 0; i <= sizeLine; i++)
		if(max_strokPixLen < GRAPHICS_lengtchPixStroke((char*)conf_meny->lineMeny[numline].list_param[i], (FontDef_t*)font))
			max_strokPixLen = GRAPHICS_lengtchPixStroke((char*)conf_meny->lineMeny[numline].list_param[i], (FontDef_t*)font);
	max_strokPixLen += 10; // добавляем 10 пикселей, будет подсветка отображатся бульше чем сама трока по краям на один символ

	// смещения всех строк, зависит от количество параметров
	uint8_t offset_line_putch;
	if(sizeLine == 1)
		 offset_line_putch = BORDER_TOP_WINDOWS + ((font->FontHeight/2)+2) * 2; //  если всего два параметра, 0 и 1, то смещяем больше
	else offset_line_putch = BORDER_TOP_WINDOWS + font->FontHeight;  //  если параметров 3 и более то отображается всего три параметра, и смещения будет таким

	draw_topRectangle(BORDER_TOP_WINDOWS);
	statusButton = PRESS_NO; // сбрасываем флаг нажатия кнопки энкодера
    // рисуем заголовок
	draw_title(conf_meny, numline);

	encoder = *(uint8_t*)conf_meny->lineMeny[numline].value;
	menu_writeControls(encoder); // в энкодер загружаем выбраный параметр

	// определяем состояние отрисовки строк по исходному параметру
	if(encoder > sizeShow_line){
		offset = encoder - sizeShow_line;
		lineBacklight = sizeShow_line;
	}
	// если количество строк меньше максимально отображаемых
	if(sizeLine < sizeShow_line) sizeShow_line = sizeLine;

    while(1)
    {
    	erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поверхностный квадрат
	    encoder = menu_сontrols_limit(0, sizeLine, &statusButton);

		lineBacklight = encoder - offset; // находим номер выбранной строки с учетом смещения экрана
		if(lineBacklight > sizeShow_line){ offset++; lineBacklight = sizeShow_line;} // если разница больше чем отображае количество строк то сдвигаем список строк вверх
		if(lineBacklight < 0){ offset--; lineBacklight = 0;} // если разница меньше то сдвигаем список строк ввниз

		// отрисовываем строки меню, проходимся по всему масиву строк и отрисовываем только видимые
		for(int i = 0; i <= sizeLine; i++){
			posline = i - offset; // определяем, находится ли эта строка в видемом участке
			if(posline <= sizeShow_line) // posline - позиция строки где надо рисовать, i - номер строки из масива
				GRAPHICS_centerLine_Puts((posline * font->FontHeight) + offset_line_putch, (char*)conf_meny->lineMeny[numline].list_param[i], (FontDef_t*)font, GRAPHICS_COLOR_WHITE);
		}
		// отрисовываем подсветку выделенной строки, также выводим относительно центра экрана!
		uint8_t x_pixel = (GRAPHICS_WIDTH - max_strokPixLen) / 2;
        GRAPHICS_DrawFilledRectangle(x_pixel , offset_line_putch - 1 + font->FontHeight * lineBacklight, max_strokPixLen,  font->FontHeight, GRAPHICS_COLOR_INVERT);

		if(statusButton == PRESS_LONG){  // выход из меню
			// очистка области отрисовки меню
			GRAPHICS_DrawFilledRectangle(BORDER_TOP_WINDOWS - 4, BORDER_TOP_WINDOWS - 6, GRAPHICS_WIDTH - (BORDER_TOP_WINDOWS*2) + 5, GRAPHICS_HEIGHT - (BORDER_TOP_WINDOWS*2) + 6, GRAPHICS_COLOR_BLACK);
		    menu_writeControls(numline);   // обратно записываем в энкодер номер строки меню где находились
			return;
		}

	    if(statusButton == PRESS_FAST) // если кнопка нажата, сохраняем наш выбор
	    {
			GRAPHICS_DrawFilledRectangle(BORDER_TOP_WINDOWS - 4, BORDER_TOP_WINDOWS - 6, GRAPHICS_WIDTH - (BORDER_TOP_WINDOWS*2) + 5, GRAPHICS_HEIGHT - (BORDER_TOP_WINDOWS*2) + 6, GRAPHICS_COLOR_BLACK);
	    	*(uint8_t*)conf_meny->lineMeny[numline].value = lineBacklight + offset;

	    	menu_writeControls(numline);   // обратно записываем в энкодер номер строки меню где находились
			//Если есть пост функция то запускаем ее
			 if(conf_meny->lineMeny[numline].post_func != NONE){
				 void (*func_ptr)() = (void (*)())conf_meny->lineMeny[numline].post_func;
				 if(conf_meny->lineMeny[numline].typeValue == t_text_list) func_ptr(*((int16_t*)conf_meny->lineMeny[numline].value));
			 }

			return;
	    }
		GRAPHICS_UpdateScreen();
    }
}
