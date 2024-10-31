/*
 * module_date_time.c
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#include "module_date_time.h"
#include "../meny.h"

void add_lineMenu_time(config_meny *config,  char* name1, char* name2, RTC_TimeTypeDef* value, void* postFunc)
{
	if(value->Hours > 24)   value->Hours = 24;
	if(value->Minutes > 59) value->Minutes = 59;
	if(value->Seconds > 59) value->Seconds = 59;

	strcpy(config->lineMeny[config->sizeLine].name_1, name1);
	strcpy(config->lineMeny[config->sizeLine].name_2, name2);
	config->lineMeny[config->sizeLine].value = value;
	config->lineMeny[config->sizeLine].minValue = NONE;
	config->lineMeny[config->sizeLine].maxValue = NONE;
	config->lineMeny[config->sizeLine].post_func = postFunc;
	config->lineMeny[config->sizeLine].typeValue = t_time;
	config->sizeLine++;
}

void add_lineMenu_date(config_meny *config,  char* name1, char* name2, Date_TypeDef* value, void* postFunc)
{
	if(value->day > 31) value->day = 31;
	if(value->month > 12) value->month = 12;
	if(value->year < 2024) value->year = 2024;
	if(value->year > 2099) value->year = 2099;
	if(value->week > 6) value->week = 6;

	strcpy(config->lineMeny[config->sizeLine].name_1, name1);
	strcpy(config->lineMeny[config->sizeLine].name_2, name2);
	config->lineMeny[config->sizeLine].value = value;
	config->lineMeny[config->sizeLine].minValue = NONE;
	config->lineMeny[config->sizeLine].maxValue = NONE;
	config->lineMeny[config->sizeLine].post_func = postFunc;
	config->lineMeny[config->sizeLine].typeValue = t_date;
	config->sizeLine++;
}

void time_to_string(RTC_TimeTypeDef btime, char *buf)
{
	 sprintf(buf,     "%02u", btime.Hours);
	 sprintf(&buf[3], "%02u", btime.Minutes);
	 sprintf(&buf[6], "%02u", btime.Seconds);
	 buf[2] = ':';
	 buf[5] = ':';
}

void date_to_string(Date_TypeDef sDate, char *buf)
{
	 sprintf(buf,     "%02u", sDate.day);
	 sprintf(&buf[3], "%02u", sDate.month);
	 sprintf(&buf[6], "%04u", sDate.year);
	 buf[2] = '.';
	 buf[5] = '.';
}


// конвертирует тип переменной время в ASCII
uint8_t time_convert_changeStrok(const config_meny *conf_meny, uint8_t numStroke, char* ext_buffer)
{
     char  buffer[MAX_SYMBOL_BUF];
	 uint8_t  length_value;     // максимальная длина числа
	 uint8_t  length_maxValue;
	 RTC_TimeTypeDef value;

	 value.Hours =   (*((RTC_TimeTypeDef*)conf_meny->lineMeny[numStroke].value)).Hours;
	 value.Minutes = (*((RTC_TimeTypeDef*)conf_meny->lineMeny[numStroke].value)).Minutes;
	 value.Seconds = (*((RTC_TimeTypeDef*)conf_meny->lineMeny[numStroke].value)).Seconds;

     // Преобразуем число в строку:
	 // С начало преобразуем maxValue для того что бы узнать максимальную длинну числа и подготовить
	 // буфер под эту длину, и заполнить его нулями. Потом преобразуем value и копируем его в буфер начиная с конца
	 // если спереди останется место то там будут нули, это важно для редактирование переменной!

	 // Длинна строки у нас постоянная
	 length_maxValue = 9;
	 length_value = 9;

	 sprintf(buffer,     "%02u", value.Hours);
	 sprintf(&buffer[3], "%02u", value.Minutes);
	 sprintf(&buffer[6], "%02u", value.Seconds);
	 buffer[2] = ':';
	 buffer[5] = ':';

	 // заполняем рабочую область буфера нулями
	 for(int i = 0; i<length_maxValue; i++) ext_buffer[i] = 0x30;
	 ext_buffer[length_maxValue] = '\0'; // конец буфера обозначаем концом строки
	 // копируем число ASCII в новый буфер с указанной длинной
	 int temp = length_value;
	 for(int i = length_maxValue; i>=0; i--){
	  	if(temp >= 0){ ext_buffer[i] = buffer[temp]; temp--;}
	 }
     return length_maxValue; // возвращаем длину строки в буфере
}

uint8_t date_convert_changeStrok(const config_meny *conf_meny, uint8_t numStroke, char* ext_buffer)
{
    char  buffer[MAX_SYMBOL_BUF];
	 uint8_t  length_value;     // максимальная длина числа
	 uint8_t  length_maxValue;
	 Date_TypeDef value;

	 value.day =   (*((Date_TypeDef*)conf_meny->lineMeny[numStroke].value)).day;
	 value.month = (*((Date_TypeDef*)conf_meny->lineMeny[numStroke].value)).month;
	 value.year  = (*((Date_TypeDef*)conf_meny->lineMeny[numStroke].value)).year;
	 value.year = value.year - 2000;

	 // Длинна строки у нас постоянная
	 length_maxValue = 9;
	 length_value = 9;

	 sprintf(buffer,     "%02u", value.day);
	 sprintf(&buffer[3], "%02u", value.month);
	 sprintf(&buffer[6], "%02u", value.year);
	 buffer[2] = '.';
	 buffer[5] = '.';

	 // заполняем рабочую область буфера нулями
	 for(int i = 0; i<length_maxValue; i++) ext_buffer[i] = 0x30;
	 ext_buffer[length_maxValue] = '\0'; // конец буфера обозначаем концом строки
	 // копируем число ASCII в новый буфер с указанной длинной
	 int temp = length_value;
	 for(int i = length_maxValue; i>=0; i--){
	  	if(temp >= 0){ ext_buffer[i] = buffer[temp]; temp--;}
	 }
    return length_maxValue; // возвращаем длину строки в буфере
}



RTC_TimeTypeDef stroke_to_time(char* buffer)
{
	RTC_TimeTypeDef timebuf;
	timebuf.Hours = (buffer[0] - '0') * 10;
	timebuf.Hours = timebuf.Hours + (buffer[1] - '0');
	timebuf.Minutes = (buffer[3] - '0') * 10;
	timebuf.Minutes = timebuf.Minutes + (buffer[4] - '0');
	timebuf.Seconds = (buffer[6] - '0') * 10;
	timebuf.Seconds = timebuf.Seconds + (buffer[7] - '0');
	return timebuf;
}

Date_TypeDef stroke_to_date(char* buffer)
{
	Date_TypeDef timebuf;
	timebuf.day = (buffer[0] - '0') * 10;
	timebuf.day = timebuf.day + (buffer[1] - '0');
	timebuf.month = (buffer[3] - '0') * 10;
	timebuf.month = timebuf.month + (buffer[4] - '0');
	timebuf.year = (buffer[6] - '0') * 10;
	timebuf.year = timebuf.year + (buffer[7] - '0');
	timebuf.year = timebuf.year + 2000;
	return timebuf;
}


// отрисовка и управление окна настройки числового параметра
void change_number_time_date(const config_meny *conf_meny, char* buffer, uint16_t x, uint16_t y, uint8_t numline)
{
	//uint8_t numChange = 0; // номер редактируемого элемента
	uint8_t length = 0;
	int16_t  encoder;
	//int16_t saveEncoder = 0;
	int8_t redactNum = 0;
	uint8_t pixLenSpice = 0;
	uint8_t pixLenChar = 0;// ширина линии символа который нужно подчеркнуть
	uint8_t posNum = 0;
	uint8_t cnt = 0;   // счетчик для вычислений
	uint8_t update_display = 1;

	FontDef_t *font = conf_meny->Font;
	FontDef_t *fontBig = conf_meny->FontBig;
	//TypeValue_TypeDef typeValu = conf_meny.lineMeny[numline].typeValue;

	pressButton_TypeDef statusButton = PRESS_NO;

	//for(uint8_t i = 0; i<MAX_SYMBOL_BUF ; i++) if(buffer[i] == '\0'){ length = i; break; }
	length = 3; // у нас 3 редактируемые значения и кнопка ок
	menu_writeControls(length -1); // при входе позицию редактирование ставим в самый младший разряд

    while(statusButton != PRESS_LONG ){ // вход в режим выбора элемента, выходим при длинном нажатии
	   // statusButton = readButtonStatus(button);
	   // redactNum = menu_encoderRead_limit(0, length-1 + 1); // добавляем позицию для кнокпи ок

	    redactNum = menu_сontrols_limit(0, length, &statusButton);

	    posNum = redactNum * 3; // позиция первого числа для резактирования
	    erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поле перед выводом
		GRAPHICS_GotoXY(x, y);
		GRAPHICS_Puts(buffer, fontBig, GRAPHICS_COLOR_WHITE); // обнавляем значени на экране

		// рисуем кнопку "ок"
		// очищаем место под рисование кнопеки
		GRAPHICS_DrawFilledRectangle(100 - 2, 50 - 2, 14 + 3, 10 + 3, GRAPHICS_COLOR_BLACK);
		GRAPHICS_GotoXY(101, 51);
		GRAPHICS_Puts("ok", font, GRAPHICS_COLOR_WHITE);

		if(redactNum == length) // если выбрали кнопк "ok" то выдиляем её рамкой
			GRAPHICS_DrawRectangle(100 - 2, 50 - 2, 14 + 3, 10 + 3, GRAPHICS_COLOR_INVERT );
		else// Снизу рисуем подчеркивание для обозначения выбранного элемента
		{
			cnt = 0;   // счетчик для вычислений
			// Ширина символа у каждого своя, поэтому вычисляем ширину подчеркивание под каждый символ
			pixLenChar = GRAPHICS_lengtchPixChar(buffer[posNum], fontBig);
			pixLenChar += GRAPHICS_lengtchPixChar(buffer[posNum +1], fontBig);
			pixLenSpice = 0;
			// находим растояния от начало строки до подчеркивани
			for(int i =0; i<redactNum; i++)
			{
				pixLenSpice += GRAPHICS_lengtchPixChar(buffer[cnt], fontBig);
				cnt++;
			    pixLenSpice += GRAPHICS_lengtchPixChar(buffer[cnt], fontBig);
			    cnt++;
			    pixLenSpice += GRAPHICS_lengtchPixChar(buffer[cnt], fontBig); //прибавляем ширину символа двоеточие
			    cnt++;
			}
			GRAPHICS_DrawFilledRectangle(x + pixLenSpice - (fontBig->Spice/2), y + fontBig->FontHeight, pixLenChar, 1, GRAPHICS_COLOR_INVERT);
		}
		// если нажали на кнопку "ок" то это тоже самое что длинное нажатие - выход из подменю
		if(redactNum == length) if(statusButton == PRESS_FAST) statusButton = PRESS_LONG;

	    if(statusButton == PRESS_FAST){ // кратковременное нажатие входим в режим редактирование элемента
	    	statusButton = PRESS_NO;
	    	menu_writeControls(0);
        	update_display = 1;

	        while(statusButton == PRESS_NO ) // вход в режим редактирования числа
	        {
	        	//saveEncoder = 0xFFAA; //сбрасываем число сохраненого энкодера, для обновления экрана, число можно записывать любое почти )
	        	//statusButton = readButtonStatus(button);
	        	//encoder = menu_encoderRead();

	        	encoder = menu_readControls(&statusButton);

				if(encoder != 0)// если крутанули энкодер
				{
					update_display = 1;
					//if(encoder > 0) num++; else num--;
					if(conf_meny->lineMeny[numline].typeValue == t_time) // если редактируем время
					{
						int8_t Hours;
						int8_t Minutes;
						int8_t Seconds;

						// ASCII в число
						Hours = (buffer[0] - '0') * 10;
						Hours = Hours + (buffer[1] - '0');
						Minutes = (buffer[3] - '0') * 10;
						Minutes = Minutes + (buffer[4] - '0');
						Seconds = (buffer[6] - '0') * 10;
						Seconds = Seconds + (buffer[7] - '0');

						if(posNum == 0)   Hours += encoder;
						if(posNum == 3)   Minutes += encoder;
						if(posNum == 6)   Seconds += encoder;
#ifdef MODE_ONE_BUTTON // то ходим по кругу числа, без увеличения старжего разряда
						if(Seconds > 59){ Seconds = 0; }
						if(Seconds < 0) { Seconds = 59; }
						if(Minutes > 59){ Minutes = 0; }
						if(Minutes < 0) { Minutes = 59;}
						if(Hours > 23)    Hours = 0;
						if(Hours < 0)     Hours = 23;
#else
						if(Seconds > 59){ Seconds = 0; Minutes++; }
						if(Seconds < 0) { Seconds = 59; Minutes--; }
						if(Minutes > 59){ Minutes = 0; Hours++; }
						if(Minutes < 0) { Minutes = 59; Hours--; }
						if(Hours > 23)    Hours = 0;
						if(Hours < 0)     Hours = 23;
#endif
						// обратно конвертируем в ASCII
						buffer[0] = '0' + (Hours / 10);
						buffer[1] = '0'+  (Hours % 10);
						buffer[3] = '0' + (Minutes / 10);
						buffer[4] = '0'+  (Minutes % 10);
						buffer[6] = '0' + (Seconds / 10);
						buffer[7] = '0'+  (Seconds % 10);
					}

					if(conf_meny->lineMeny[numline].typeValue == t_date) // если редактируем дату
					{
						uint8_t day;
						uint8_t month;
						uint8_t year;

						// ASCII в число
						day = (buffer[0] - '0') * 10;
						day = day + (buffer[1] - '0');
						month = (buffer[3] - '0') * 10;
						month = month + (buffer[4] - '0');
						year = (buffer[6] - '0') * 10;
						year = year + (buffer[7] - '0');

						if(posNum == 0)  day   += encoder;
						if(posNum == 3)  month += encoder;
						if(posNum == 6)  year  += encoder;
						if(day > 31) day = 1;
						if(day < 1) day = 31;
						if(month > 12) month = 1;
						if(month < 1) month = 12;
						if(year > 99) year = 99;
						if(year < 24) year = 24;

						// обратно конвертируем в ASCII
						buffer[0] = '0' + (day / 10);
						buffer[1] = '0'+  (day % 10);
						buffer[3] = '0' + (month / 10);
						buffer[4] = '0'+  (month % 10);
						buffer[6] = '0' + (year / 10);
						buffer[7] = '0'+  (year % 10);
					}
				}
	        	erase_topRectangle(BORDER_TOP_WINDOWS);
	    		GRAPHICS_GotoXY(x, y);
	    		GRAPHICS_Puts(buffer, fontBig, GRAPHICS_COLOR_WHITE); // обнавляем значени на экране
	    		// Поверх накладываем квадрат с инверсией для обозначение выделенного элемента
				// Ширина символа у каждого своя, поэтому вычисляем ширину подчеркивание под каждый символ
				cnt = 0;
				// Ширина символа у каждого своя, поэтому вычисляем ширину подчеркивание под каждый символ
				pixLenChar = GRAPHICS_lengtchPixChar(buffer[posNum], fontBig);
				pixLenChar += GRAPHICS_lengtchPixChar(buffer[posNum +1], fontBig);
				pixLenSpice = 0;
				// находим растояния от начало строки до подчеркивани
				for(int i =0; i<redactNum; i++)
				{
					pixLenSpice += GRAPHICS_lengtchPixChar(buffer[cnt], fontBig);
					cnt++;
				    pixLenSpice += GRAPHICS_lengtchPixChar(buffer[cnt], fontBig);
				    cnt++;
				    pixLenSpice += GRAPHICS_lengtchPixChar(buffer[cnt], fontBig); //прибавляем ширину символа двоеточие
				    cnt++;
				}
	    		GRAPHICS_DrawFilledRectangle(x + pixLenSpice - (fontBig->Spice/2), y - 1, pixLenChar, fontBig->FontHeight, GRAPHICS_COLOR_INVERT);
	        	// рисуем кнопку "ок" чтобы отображалась коректно из за очистки внутреннего квадрата
	    		GRAPHICS_GotoXY(101, 51);
	    		GRAPHICS_Puts("ok", font, GRAPHICS_COLOR_WHITE);

	    		if(update_display){ // обнавляем экран только тогда, когда произошли изменения
	    			if(GRAPHICS_isBusy() == 0)
	    			{
	    				menu_writeControls(0);
	    				encoder = 0;
	    				update_display = 0;
	    				GRAPHICS_UpdateScreen();
	    			}
	    		}
	    	}
	    	statusButton = PRESS_NO;
	    	menu_writeControls(redactNum);
	    }
	    GRAPHICS_UpdateScreen();
	 }
	// очищаем место под рисование кнопеки
	GRAPHICS_DrawFilledRectangle(100 - 2, 50 - 2, 14 + 3, 10 + 3, GRAPHICS_COLOR_BLACK);
}


void top_meny_change_time_date(const config_meny *conf_meny, uint8_t numline)
{
	 uint8_t bufLen;   // длина данных в буфера
	 uint8_t pixLen;   // длина пикселей для центровки строки на экране
	 char buffer[MAX_SYMBOL_BUF];
	 draw_topRectangle(BORDER_TOP_WINDOWS);
	 draw_title(conf_meny, numline);
	 menu_writeControls(0);
	 erase_topRectangle(BORDER_TOP_WINDOWS); // очищаем поверхностный квадрат
	 // конвертируем переменную в строку, лежит в buffer с длинной bufLen
    switch((uint8_t)conf_meny->lineMeny[numline].typeValue){
		case t_time:    bufLen = time_convert_changeStrok(conf_meny,  numline, buffer); break;
		case t_date:    bufLen = date_convert_changeStrok(conf_meny,  numline, buffer); break;
    }
   //вычисляем начало кординат строки что бы она была по центру
   pixLen = GRAPHICS_lengtchPixStroke(buffer, conf_meny->FontBig);
   bufLen = (GRAPHICS_WIDTH - pixLen)/ 2;

   change_number_time_date(conf_meny, buffer, bufLen, 27, numline);

   // после настройки параметра в ASCII мы переводим его обратно в число и записываем в переменную по указателю
		// приводим указатель к типу и меняем значения переменной по этому указателю
	switch((uint8_t)conf_meny->lineMeny[numline].typeValue){
		case t_time:    *((RTC_TimeTypeDef*)conf_meny->lineMeny[numline].value) = stroke_to_time(buffer);   break;
		case t_date:    *((Date_TypeDef*)conf_meny->lineMeny[numline].value)    = stroke_to_date(buffer);   break;
	}
	//Если есть пост функция то запускаем ее
	 if(conf_meny->lineMeny[numline].post_func != 0){
		 void (*func_ptr)() = (void (*)())conf_meny->lineMeny[numline].post_func;
		 if(conf_meny->lineMeny[numline].typeValue == t_time)  func_ptr(*((RTC_TimeTypeDef*)conf_meny->lineMeny[numline].value));
		 if(conf_meny->lineMeny[numline].typeValue == t_date)  func_ptr(*((Date_TypeDef*)conf_meny->lineMeny[numline].value));
	 }

	menu_writeControls(numline);  // обратно записываем в энкодер номер строки меню где находились
	// очищаем поверхностный квадрат
	GRAPHICS_DrawFilledRectangle(BORDER_TOP_WINDOWS - 4, BORDER_TOP_WINDOWS - 6, GRAPHICS_WIDTH - (BORDER_TOP_WINDOWS*2) + 5, GRAPHICS_HEIGHT - (BORDER_TOP_WINDOWS*2) + 6, GRAPHICS_COLOR_BLACK);
}
