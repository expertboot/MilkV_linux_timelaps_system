/*
 * param_int.c
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#include "module_int.h"
#include "../meny.h"
#include <stdint.h>

uint8_t add_lineMenu_int(config_meny *config, char* name1, char* name2, int16_t* value, int16_t min, int16_t max, void* postFunc)
{
    // Выделяем память для переменной int16_t
	int16_t *ptr_rangeValue = (int16_t *)malloc(2 * sizeof(int16_t));
    if (ptr_rangeValue == NULL) return 1; // ошибка выделения памяти

    ptr_rangeValue[0] = min;
    ptr_rangeValue[1] = max;

	strcpy(config->lineMeny[config->sizeLine].name_1, name1);
	strcpy(config->lineMeny[config->sizeLine].name_2, name2);
	config->lineMeny[config->sizeLine].value = value;
	config->lineMeny[config->sizeLine].minValue = &ptr_rangeValue[0];
	config->lineMeny[config->sizeLine].maxValue = &ptr_rangeValue[1];
	config->lineMeny[config->sizeLine].post_func = postFunc;
	config->lineMeny[config->sizeLine].typeValue = t_int;
	config->sizeLine++;
	return 0; // ok
}

void top_meny_changeInt(const config_meny *conf_meny, uint8_t numline)
{
		 uint8_t pixLen;
		 uint8_t x;
		 uint8_t bufLen;   // длина данных в буфера - максимальная длинна строки
		 int16_t encoder;
		 uint8_t redactNum = 0;
		 uint8_t y = 27;
		 int16_t *value;
		 int16_t maxValue;
		 int16_t minValue;
		 char buffer[MAX_SYMBOL_BUF];

		 pressButton_TypeDef statusButton = PRESS_NO;

		 value    =  ((int16_t*)conf_meny->lineMeny[numline].value);
		 maxValue = *((int16_t*)conf_meny->lineMeny[numline].maxValue);
		 minValue = *((int16_t*)conf_meny->lineMeny[numline].minValue);

		 if(*value > maxValue) *value = maxValue;
		 if(*value < minValue) *value = minValue;

		 draw_topRectangle(BORDER_TOP_WINDOWS);
		 draw_title(conf_meny, numline);
		 encoderWrite(0);
		 erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поверхностный квадрат

		 bufLen = int_convert_changeStrok(conf_meny,  numline, buffer);
		 bufLen++; // добавляем позицию кнопки ОК
		 pixLen = GRAPHICS_lengtchPixStroke(buffer, conf_meny->FontBig);

		 x = (GRAPHICS_WIDTH - pixLen)/ 2;

	    while(statusButton != PRESS_LONG ){ // вход в режим выбора элемента, выходим при длинном нажатии
		    redactNum = menu_сontrols_limit(0, bufLen, &statusButton);
			// если нажали на кнопку "ок" то это тоже самое что длинное нажатие - выход из подменю
			if(redactNum == bufLen) if(statusButton == PRESS_FAST) statusButton = PRESS_LONG;
		    // обнавляем значение на экране
		    erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поле перед выводом
			// рисуем кнопку "ок",  очищаем место под рисование кнопеки
			GRAPHICS_DrawFilledRectangle(100 - 2, 50 - 2, 14 + 3, 10 + 3, GRAPHICS_COLOR_BLACK);
			GRAPHICS_GotoXY(100, 51);
			GRAPHICS_Puts("ok", conf_meny->Font, GRAPHICS_COLOR_WHITE);

			GRAPHICS_centerLine_Puts(y, buffer, conf_meny->FontBig, GRAPHICS_COLOR_WHITE);
			 // если выбрали кнопк "ok" то выдиляем её рамкой
			if(redactNum == bufLen)
				GRAPHICS_DrawRectangle(100 - 2, 50 - 2, 14 + 3, 10 + 3, GRAPHICS_COLOR_INVERT );
			else{// Снизу рисуем подчеркивание для обозначения выбранного элемента
				// Ширина символа у каждого своя, поэтому вычисляем ширину подчеркивание под каждый символ
				uint8_t pixLenSpice = 0;
				uint8_t pixLenChar = GRAPHICS_lengtchPixChar(buffer[redactNum], conf_meny->FontBig);// ширина линии символа который нужно подчеркнуть
				for(int i =0; i<redactNum; i++) pixLenSpice += GRAPHICS_lengtchPixChar(buffer[i], conf_meny->FontBig); // растояния от начало строки до подчеркивани
				GRAPHICS_DrawFilledRectangle(x + pixLenSpice - (conf_meny->FontBig->Spice/2), y + conf_meny->FontBig->FontHeight, pixLenChar, 1, GRAPHICS_COLOR_INVERT);
			}

// ************************** РЕДАКТИРУЕМ СИМВОЛ ***************************************************
		    if(statusButton == PRESS_FAST){ // кратковременное нажатие входим в режим редактирование элемента

		    	statusButton = PRESS_NO; // сбрасываем фоаг кнопки
		    	menu_writeControls(0);
	        	uint8_t updateDisplay = 1;

		        while(statusButton == PRESS_NO ) // вход в режим редактирование числа
		        {
		        	encoder = menu_readControls(&statusButton);
#ifdef MODE_ONE_BUTTON
					if(encoder != 0)// если крутанули энкодер
					{
						updateDisplay = 1;
						// добавляем к ASCII числу то, что насчитал энкодер
						uint8_t pos = bufLen - redactNum; // инвертируем число для удобство
						pos--; // смещаем из за добавленного места под кнопку

						if(minValue < 0 && pos == bufLen-1) // если есть знак и встали на его редактирование
						{
							*value = -*value;
						}
						else
						{
							int16_t factor = 1;
							for (uint8_t i = 0; i < pos; i++) factor *= 10; // вычисляем фактор для прибавления к челу
							// Извлекаем разряд, который будем изменять
							int16_t value_abs = abs(*value);
							int16_t digit = (value_abs / factor) % 10;
							// Обновляем значение разряда на основе данных от энкодера
							digit += encoder;
							// Сделаем разряд цикличным (если больше 9 — переходит в 0, если меньше 0 — переходит в 9)
							if (digit > 9) {
								digit = 0;
							} else if (digit < 0) {
								digit = 9;
							}
							// Обновляем только редактируемый разряд числа, остальные разряды остаются без изменений
							value_abs = value_abs - ((value_abs / factor) % 10) * factor + digit * factor;
							if(*value < 0) *value = -value_abs; // возвращаем знак если он был
							else *value = value_abs;
						}
					}
#else
					if(encoder != 0)// если крутанули энкодер
					{
						updateDisplay = 1;
						// добавляем к ASCII числу то, что насчитал энкодер
						uint8_t pos = bufLen - redactNum; // инвертируем число для удобство
						pos--; // смещаем из за добавленного места под кнопку
						int16_t factor = 1;
						for (uint8_t i = 0; i < pos; i++) {
							factor *= 10;  // Умножение на 10 для каждого значения pos
						}
						*value = *value + (factor * encoder);

						if(*value > maxValue) *value = maxValue;
						if(*value < minValue) *value = minValue;
					}
#endif

					// отрисовка значения
				    erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поле перед выводом
				    bufLen = int_convert_changeStrok(conf_meny,  numline, buffer);
					GRAPHICS_centerLine_Puts(y, buffer, conf_meny->FontBig, GRAPHICS_COLOR_WHITE);
					bufLen++;

					if(*value >= maxValue){
						char buf_2[20] = "max";
						GRAPHICS_centerLine_Puts(15, buf_2, &Font_5x9, GRAPHICS_COLOR_WHITE);
					}
					if(*value <= minValue){
						char buf_2[20] = "min";
						GRAPHICS_centerLine_Puts(15, buf_2, &Font_5x9, GRAPHICS_COLOR_WHITE);
					}
		    		// Поверх накладываем квадрат с инверсией для обозначение выделенного элемента
					// Ширина символа у каждого своя, поэтому вычисляем ширину подчеркивание под каждый символ
					uint8_t pixLenSpice = 0;
					uint8_t pixLenChar = GRAPHICS_lengtchPixChar(buffer[redactNum], conf_meny->FontBig);// ширина линии символа который нужно подчеркнуть
					for(int i =0; i<redactNum; i++) pixLenSpice += GRAPHICS_lengtchPixChar(buffer[i], conf_meny->FontBig); // растояния от начало строки до подчеркивани
		    		GRAPHICS_DrawFilledRectangle(x + pixLenSpice - (conf_meny->FontBig->Spice/2), y - 1, pixLenChar, conf_meny->FontBig->FontHeight, GRAPHICS_COLOR_INVERT);
		        	// рисуем кнопку "ок" чтобы отображалась коректно из за очистки внутреннего квадрата
		    		GRAPHICS_GotoXY(100, 51);
		    		GRAPHICS_Puts("ok", conf_meny->Font, GRAPHICS_COLOR_WHITE);

		    		if(updateDisplay){ // обнавляем экран только тогда, когда произошли изменения
		    			if(GRAPHICS_isBusy() == GRAPHICS_REDY)
		    			{
		    				updateDisplay = 0;
		    				encoder = 0;
		    				menu_writeControls(0);
		    				GRAPHICS_UpdateScreen();
		    			}
		    		}
		    	}

		    	menu_writeControls(redactNum);
		    }

			if(*value >= maxValue){
				char buf_2[20] = "max";
				GRAPHICS_centerLine_Puts(15, buf_2, &Font_5x9, GRAPHICS_COLOR_WHITE);
			}
			if(*value <= minValue){
				char buf_2[20] = "min";
				GRAPHICS_centerLine_Puts(15, buf_2, &Font_5x9, GRAPHICS_COLOR_WHITE);
			}

		    if(GRAPHICS_isBusy() == GRAPHICS_REDY) GRAPHICS_UpdateScreen();
		 }

		if(*value > maxValue) *value = maxValue;
		if(*value < minValue) *value = minValue;

	//Если есть пост функция то запускаем ее
	 if(conf_meny->lineMeny[numline].post_func != NONE){
		 void (*func_ptr)() = (void (*)())conf_meny->lineMeny[numline].post_func;
		 if(conf_meny->lineMeny[numline].typeValue == t_int){
		  	 int16_t parametr = *((int16_t*)conf_meny->lineMeny[numline].value);
			 func_ptr(parametr);
		 }
	 }
	 menu_writeControls(numline);   // обратно записываем в энкодер номер строки меню где находились
	  // очищаем поверхностный квадрат
	 GRAPHICS_DrawFilledRectangle(BORDER_TOP_WINDOWS - 4, BORDER_TOP_WINDOWS - 6, GRAPHICS_WIDTH - (BORDER_TOP_WINDOWS*2) + 5, GRAPHICS_HEIGHT - (BORDER_TOP_WINDOWS*2) + 6, GRAPHICS_COLOR_BLACK);
}


uint8_t int_convert_changeStrok(const config_meny *conf_meny, uint8_t numline, char* ext_buffer)
{
	 char buffer[MAX_SYMBOL_BUF];
	 uint8_t length_value;     // максимальная длина числа
	 uint8_t length_maxValue;
	 uint8_t length_minValue;
	 int16_t value;
	 int16_t maxValue;
	 int16_t minValue;

	 value =    *((int16_t*)conf_meny->lineMeny[numline].value);
	 maxValue = *((int16_t*)conf_meny->lineMeny[numline].maxValue);
	 minValue = *((int16_t*)conf_meny->lineMeny[numline].minValue);
     // Преобразуем число в строку:
	 // С начало преобразуем maxValue для того что бы узнать максимальную длинну числа и подготовить
	 // буфер под эту длину, и заполнить его нулями. Потом преобразуем value и копируем его в буфер начиная с конца
	 // если с переди останется место то там будут нули, это важно для редактирование переменной!
	 // узнаем максимальную длинну символов
     sprintf(buffer, "%i", abs(maxValue)); length_maxValue = strlen(buffer);
	 sprintf(buffer, "%i", abs(minValue)); length_minValue = strlen(buffer);
	 sprintf(buffer, "%i", abs(value));    length_value = strlen(buffer);

	 if(minValue < 0) length_maxValue++; // резервируем место под знак
	 if(length_maxValue < length_minValue) length_maxValue = length_minValue; // определяем максимальную длинну символов числа

	 // заполняем рабочую область буфера нулями
	 for(int i = 0; i<length_maxValue; i++) ext_buffer[i] = 0x30;
	 ext_buffer[length_maxValue] = '\0'; // конец буфера обозначаем концом строки

	 length_maxValue = length_maxValue - 1; // переходим в режим указателя в буфере а не его размер

	// if(minValue < 0){ // если это знаковая переменная
		 if(value < 0) ext_buffer[0] = '-';
		 else ext_buffer[0] = ' ';
	// }

	 int temp = length_value - 1;
     // заполняем буфер, сначало знак, потом нули по размеру переменгой потом само число
	 for(int i = length_maxValue; i>=0; i--){
	  	if(temp >= 0){
	  		ext_buffer[i] = buffer[temp];
	  		temp--;
	  	}
	 }

    return length_maxValue++; // возвращаем длинну строки в буфере
}

/*
int16_t stroke_to_int16(char* buffer)
{
	int32_t value = 0; // число
	uint8_t sign = 0; // знак числа 0- положительное, 1 — отрицательное
	uint8_t i = 1; // указатель
	if(buffer[0] == '-') sign = 1;
	while(buffer[i] >= 0x30 && buffer[i] <= 0x39){ // проверка на наличия цыфры
		value = value + (buffer[i] & 0x0F); // преобразуем в число и заносим в младший разряд value
		value = value * 10; // сдвигием разряд в лево
	    i++;
	}
	value = value / 10; // после окончания сдвигиаем обратно разряд
	if (sign == 1) value = -value;
	if(value > INT_LEAST16_MAX) value = INT_LEAST16_MAX; // проверка на пределы значение регистра
	if(value < INT_LEAST16_MIN) value = INT_LEAST16_MIN;
	return value;
}*/
