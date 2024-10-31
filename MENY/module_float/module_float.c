/*
 * module_float.c
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#include "module_float.h"
#include "../meny.h"

uint8_t add_lineMenu_float(config_meny *config, char* name1, char* name2, float* value, float min, float max, void* postFunc)
{
    // Выделяем память для переменной float
	float *ptr_rangeValue = (float *)malloc(2 * sizeof(float));
    if (ptr_rangeValue == NULL) return 1; // ошибка выделения памяти

    ptr_rangeValue[0] = min;
    ptr_rangeValue[1] = max;

	strcpy(config->lineMeny[config->sizeLine].name_1, name1);
	strcpy(config->lineMeny[config->sizeLine].name_2, name2);
	config->lineMeny[config->sizeLine].value = value;
	config->lineMeny[config->sizeLine].minValue = &ptr_rangeValue[0];
	config->lineMeny[config->sizeLine].maxValue = &ptr_rangeValue[1];
	config->lineMeny[config->sizeLine].post_func = postFunc;
	config->lineMeny[config->sizeLine].typeValue = t_float;
	config->sizeLine++;
	return 0; // ok
}


void top_meny_changeFloat(const config_meny *conf_meny, uint8_t numline)
{
	 uint8_t pixLen;
	 uint8_t x;
	 uint8_t bufLen;   // длина данных в буфера - максимальная длинна строки
	 int16_t encoder;
	 uint8_t redactNum = 0;
	 uint8_t y = 27;
	 float *value;
	 float maxValue;
	 float minValue;
	 uint8_t dot_pos;
	 char buffer[MAX_SYMBOL_BUF];

	 pressButton_TypeDef statusButton = PRESS_NO; // сбрасываем флаг нажатия кнопки энкодера

	 value    =  ((float*)conf_meny->lineMeny[numline].value);
	 maxValue = *((float*)conf_meny->lineMeny[numline].maxValue);
	 minValue = *((float*)conf_meny->lineMeny[numline].minValue);

	 if(*value > maxValue) *value = maxValue;
	 if(*value < minValue) *value = minValue;

	 draw_topRectangle(BORDER_TOP_WINDOWS);
	 draw_title(conf_meny, numline);
	 encoderWrite(0);
	 erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поверхностный квадрат

	 bufLen = float_convert_changeStrok(conf_meny, numline, buffer);
	 bufLen++; // добавляем позицию кнопки ОК
	 pixLen = GRAPHICS_lengtchPixStroke(buffer, conf_meny->FontBig);
	 bufLen--; // убираем в счетчики точку
	 // поиск позиции точки
	 for(int i =0; i<bufLen; i++) if(buffer[i] == ',') {dot_pos = i; break;}

	 x = (GRAPHICS_WIDTH - pixLen)/ 2;

	   while(statusButton != PRESS_LONG )// вход в режим выбора элемента, выходим при длинном нажатии
	   {
			redactNum = menu_сontrols_limit(0, bufLen-1, &statusButton);

			if(redactNum > dot_pos - 1) redactNum++;

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
			if(statusButton == PRESS_FAST)
			{ // кратковременное нажатие входим в режим редактирование элемента
				statusButton = PRESS_NO;
				menu_writeControls(0);
				uint8_t updateDisplay = 1;
					while(statusButton == PRESS_NO ) // вход в режим редактирование числа
					{
						encoder = menu_readControls(&statusButton);
						if(encoder != 0)// если крутанули энкодер
						{
							updateDisplay = 1;

							if(redactNum == 0)
							{
								if(buffer[0] == ' ') buffer[0] = '-';
								else buffer[0] = ' ';
							}else
							{
								// добавляем к ASCII числу то, что насчитал энкодер
								if(encoder > 0) incriment_ascii(buffer, redactNum, encoder);
								else decriment_ascii(buffer, redactNum, abs(encoder));
							}

							*value = stroke_to_float(buffer);

							if(*value > maxValue) *value = maxValue;
							if(*value < minValue) *value = minValue;
						}
						// отрисовка значения
						erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поле перед выводом
						//bufLen = float_convert_changeStrok(conf_meny,  numline, buffer);
						GRAPHICS_centerLine_Puts(y, buffer, conf_meny->FontBig, GRAPHICS_COLOR_WHITE);

						if(*value == maxValue){
							char buf_2[20] = "max";
							GRAPHICS_centerLine_Puts(15, buf_2, &Font_5x9, GRAPHICS_COLOR_WHITE);
						}
						if(*value == minValue){
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

					//*value = stroke_to_float(buffer);
					statusButton = PRESS_NO;
					if(redactNum > dot_pos - 1) redactNum--;
					menu_writeControls(redactNum);
				}

			if(*value == maxValue){
				char buf_2[20] = "max";
				GRAPHICS_centerLine_Puts(15, buf_2, &Font_5x9, GRAPHICS_COLOR_WHITE);
			}
			if(*value == minValue){
				char buf_2[20] = "min";
				GRAPHICS_centerLine_Puts(15, buf_2, &Font_5x9, GRAPHICS_COLOR_WHITE);
			}

			if(GRAPHICS_isBusy() == GRAPHICS_REDY) GRAPHICS_UpdateScreen();
		 }
		//Если есть пост функция то запускаем ее
		if(conf_meny->lineMeny[numline].post_func != NONE){
			 void (*func_ptr)() = (void (*)())conf_meny->lineMeny[numline].post_func;
			 if(conf_meny->lineMeny[numline].typeValue == t_int)   func_ptr(*((int16_t*)conf_meny->lineMeny[numline].value));
		}

		statusButton = PRESS_NO; // сбрасываем флаг нажатия кнопки энкодера
		menu_writeControls(numline);   // обратно записываем в энкодер номер строки меню где находились
		 // очищаем поверхностный квадрат
		GRAPHICS_DrawFilledRectangle(BORDER_TOP_WINDOWS - 4, BORDER_TOP_WINDOWS - 6, GRAPHICS_WIDTH - (BORDER_TOP_WINDOWS*2) + 5, GRAPHICS_HEIGHT - (BORDER_TOP_WINDOWS*2) + 6, GRAPHICS_COLOR_BLACK);
}

// возвращает максимальное возможное количество сиволов, включая знак
uint8_t float_convert_changeStrok(const config_meny *conf_meny, uint8_t numStroke, char* ext_buffer)
{
     char buffer[MAX_SYMBOL_BUF ];
	 uint8_t length_value;     // максимальная длина числа
	 uint8_t length_maxValue;
	 uint8_t dot_value;        // позиция запятой
	 uint8_t dot_maxValue;
	 float value    = *((float*)conf_meny->lineMeny[numStroke].value);
	 float maxValue = *((float*)conf_meny->lineMeny[numStroke].maxValue);
	 float minValue = *((float*)conf_meny->lineMeny[numStroke].minValue);
     // Преобразуем число в строку:
     float_to_string(fabs(maxValue), buffer, 1);
	 for(uint8_t i = 0; i<MAX_SYMBOL_BUF ; i++) if(buffer[i] == '\0'){ length_maxValue = i; break; }
 	 // находим позицию запятой начиная с конца
 	 for(int i = length_maxValue; i>=0; i--) if(buffer[i] == ','){ dot_maxValue = length_maxValue - i; break; }
     //----------------
 	 if(value < 0.00f && value > -0.001) // если float = 0
 	 {
 		ext_buffer[0] = '0';
 		ext_buffer[1] = ',';
 		ext_buffer[2] = '0';
 		ext_buffer[3] = '\0'; // конец буфера обозначаем концом строки
 		length_maxValue = 4;
 	 }
 	 else
 	 {
		 float_to_string(fabs(value), buffer, 1);
		 for(uint8_t i = 0; i<MAX_SYMBOL_BUF ; i++) if(buffer[i] == '\0'){ length_value = i; break;}
		 // находим позицию запятой начиная с конца
		 for(int i = length_value; i>=0; i--) if(buffer[i] == ','){ dot_value = length_value - i; break; }
		 // заполняем рабочую область буфера нулями
		 for(int i = 0; i<length_maxValue; i++) ext_buffer[i] = 0x30;
		 ext_buffer[length_maxValue] = '\0'; // конец буфера обозначаем концом строки
 	 }
	 // если число может быть отрицательным
	 if(minValue < 0){
		 ext_buffer[length_maxValue] = 0x30; // смещаем конец строки, инциализируем предыдущую ячеку
		 length_maxValue++;
		 ext_buffer[length_maxValue] = '\0'; // смещаем конец строки// добавляем место под знак
		 if(value < 0)ext_buffer[0] = '-';   // инциализируем место под знак
		 else ext_buffer[0] = ' ';
	 }
	 int temp = length_value - 1;
	 uint8_t offset_dot = 0; // смещения числа при копировании во внешний буфер
	 if(dot_value > dot_maxValue){ offset_dot = 0; temp = temp - (dot_value - dot_maxValue); };
	 if(dot_value < dot_maxValue) offset_dot = dot_maxValue - dot_value;
	 if(minValue < 0){
		 for(int i = length_maxValue - 1 - offset_dot; i>0; i--){
			 if(temp >= 0){ext_buffer[i] = buffer[temp];temp--;}}
	 } else{

		 for(int i = length_maxValue - 1 - offset_dot; i>=0; i--){
			 if(temp >= 0){ ext_buffer[i] = buffer[temp];temp--;}
		 }
	 }
	  return length_maxValue;
}


// строку в флоат
float stroke_to_float(char* buffer)
{
	int value = 0;       // число
	uint8_t sign = 0;    // знак числа 0- положительное, 1 — отрицательное
	uint8_t posDot = 0;  // позиция точки
	uint8_t i = 1;       // указатель
	float out;
	uint8_t buflen = strlen(buffer);
	if(buffer[0] == '-') sign = 1;

	for( ; i < buflen; i++)
	{
		if(buffer[i] == ',') posDot = i;
		else {
			value = value + (buffer[i] & 0x0F); // преобразуем в число и заносим в младший разряд value
			value = value * 10;                 // сдвигием разряд в лево
		}
	}
	value = value / 10; // после окончания сдвигиаем обратно разряд
	// формируем запятую
	if(posDot != 0){
		posDot = i - posDot - 1; // -1 компенсируем пустую ячейку под символ минуса
		uint32_t r = 1;
		for(uint16_t t = 0; t < posDot; t++) r = r *10; // собираем делитель для получения запятой в нужном месте
		out = (float)value / r;
	} else out = (float)value;

	if (sign == 1) out  = -out ;
	return out ;
}


// увеличивает число в формате ASCII
// pos_number - позиция цивры в буфере которое редактируется
// cnt - число которое надо прибавить
void incriment_ascii(char* buffer, uint8_t pos_number, uint16_t cnt)
{

	uint16_t a = buffer[pos_number] - '0'; // ACII в число

#ifndef MODE_ONE_BUTTON
	a = a + cnt;

	while(a > 9)
	{
		a = a - 10;
		// цикл инкремента ASCII числа
		for(int i = pos_number - 1; i > 0; i--)
		{
		    if(buffer[i] != ',')
			if(buffer[i] != ' ')
			if(buffer[i] != '-')
			{
				if(buffer[i] < '0') break; // если нету ASCII числа в буфере, выходим

				uint16_t b = buffer[i] - '0'; // читаем число
				b++;
				if(b > 9) buffer[i] = '0';    // если перешли на новый разряд, то продолжаем
				else {
					 buffer[i] = b + '0';
					 break;
				}
			}
		}
	} // выходим, a <= 9

#else
	a = (a + cnt) % 10;
#endif
	buffer[pos_number] = a + '0';
}


// уменьшает число в формате ASCII
// pos_number - позиция цивры в буфере которое редактируется
// cnt - число которое надо вычисть
void decriment_ascii(char* buffer, uint8_t pos_number, uint16_t cnt)
{
	int16_t a = buffer[pos_number] - '0'; // ACII в число

#ifndef MODE_ONE_BUTTON
	a = a - cnt;

	while(a < 0)
	{
		a = a + 10;
		// цикл инкремента ASCII числа
		for(int i = pos_number - 1; i > 0; i--)
		{
		    if(buffer[i] != ',')
			if(buffer[i] != ' ')
			if(buffer[i] != '-')
			{
				if(buffer[i] < '0') break; // если нету ASCII числа в буфере, выходим

				int16_t b = buffer[i] - '0'; // читаем число
				b--;
				if(b < 0) buffer[i] = '0' + 9;    // если перешли на новый разряд, то продолжаем
				else {
					 buffer[i] = b + '0';
					 break;
				}
			}
		}
	} // выходим, a <= 9

#else
	a = (a - cnt) % 10;
#endif
	buffer[pos_number] = a + '0';
}


int n_tu1(int number, int count)
{

    int result=1;
    while(count-- > 0)
    result *= number;
    return result;
}
/***Конвертор float в строку***/
void float_to_string(float f, char *r, uint8_t size_after_dot)
{
    long long int length, length2, i, number, position, sign;
    float number2;

    sign = -1;   // -1 == positive number
    if (f < 0){
        sign = '-';
        f *= -1;
    }
    number2 = f;
    number = f;
    length = 0;  // size of decimal part
    length2 = 0; //  size of tenth
    /* calculate length2 tenth part*/
    while( (number2 - (float)number) != 0.0 && !((number2 - (float)number) < 0.0) ){
         number2 = f * (n_tu1(10.0, length2 + 1));
         number = number2;
         length2++;
    }
    // ограничиваем длину знаков после запятой
    if(length2 > size_after_dot) length2 = size_after_dot;

    /* calculate length decimal part*/
    for(length = (f > 1) ? 0 : 1; f > 1; length++)
        f /= 10;

    position = length;
    length = length + 1 + length2;
    number = number2;
    if(sign == '-'){
        length++;
        position++;
    }
   // if(length > lengthNum) length = lengthNum;
     for(i = length; i >= 0 ; i--)
    {
    	 if(i == (length))
            r[i] = '\0';
        else if(i == (position))
            r[i] = ',';
        else if(sign == '-' && i == 0)
            r[i] = '-';
        else
        {
            r[i] = (number % 10) + '0';
            number /=10;
        }
         // рисуем ноль после запятой, если там нет цифры
    	 if(r[length-1] == ',')
    	 {
    		 r[length] = '0';
    		 r[length+1] = '\0';
    	 }
    }

}

