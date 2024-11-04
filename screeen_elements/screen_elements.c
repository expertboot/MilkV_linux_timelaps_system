/*
 * screen_elements.c
 *
 *  Created on: 23 сент. 2024 г.
 *      Author: ro_rudenko
 */


#include "screen_elements.h"
#include <stdint.h>
//#include <stdlib.h>

extern config_meny config;
extern button_TypeDef button_encoder;
extern struct param settings;
//extern Am2320_HandleTypeDef Am2320_;


void screen_main(struct param* settings)
{
	pressButton_TypeDef statusButton;
	RTC_TimeTypeDef sTimeTime;
	RTC_TimeTypeDef sTime = {0};
	char buffer[20];

	printf("Action Week: %02X\n", settings->action_week);
	while(statusButton != PRESS_LONG)
	{
		statusButton = readButtonStatus(&button_encoder);

		GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);

		GRAPHICS_GotoXY(0,0);
		sTimeTime = ds3231_read_rtc();

		sprintf(buffer, "%02u:%02u:%02u ", sTimeTime.Hours, sTimeTime.Minutes, sTimeTime.Seconds);
		GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);

		GRAPHICS_GotoXY(58,0);
		Date_TypeDef sDate = ds3231_read_date();
		sprintf(buffer, "%02u-%02u-%04u", sDate.day, sDate.month, sDate.year);
		GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);

		//screen_set_week()
	//	printf("Action Week: %02X\n", settings.action_week);
		draw_box_week(12, settings->action_week);

		sTime = settings->time_interval;
	    GRAPHICS_GotoXY(0,36);

		//sTime.Seconds++;

		sprintf(buffer, "INTERVAL %02u:%02u:%02u", sTime.Hours, sTime.Minutes, sTime.Seconds);
		//GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);
		GRAPHICS_centerLine_Puts(36, buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);

		sTime = settings->time_interval;
		//GRAPHICS_DrawRectangle(0, 48 , 128 , 14, GRAPHICS_COLOR_WHITE);
		GRAPHICS_centerLine_Puts(51, " START  SETUP ", &Font_10x10, GRAPHICS_COLOR_WHITE); \

	    
		uint8_t enc = encoderRead_limit(0, 1);

	    if(enc == 0)
	    	GRAPHICS_DrawFilledRectangle(14, 48, 44, 14, GRAPHICS_COLOR_INVERT); // START
	    else{
	    	GRAPHICS_DrawFilledRectangle(69 ,48, 41 , 14, GRAPHICS_COLOR_INVERT);  // SETTINGS
	    }

	    if(statusButton == PRESS_FAST)
	    {
	    	if(enc == 1)
			{
				menu_draw(&config);
				printf("Action Week: %02X\n", settings->action_week);
			}
	    	else{
	//    		start_run();
	    		statusButton = PRESS_NO;
	    	}


	    } else GRAPHICS_UpdateScreen();
	}
}




void screen_test_system(void)
{

}

void screen_sensor(void)
{
	pressButton_TypeDef statusButton = PRESS_NO;
	char buffer[20];
	float temperature, humidity;
	RTC_TimeTypeDef sTime = {0};

	set_power_camera(1);
	while(statusButton == PRESS_NO)
	{
		statusButton = readButtonStatus(&button_encoder);

		GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);

		GRAPHICS_GotoXY(0,0);
		sTime = ds3231_read_rtc();
		sprintf(buffer, "%02u:%02u:%02u ", sTime.Hours, sTime.Minutes, sTime.Seconds);
		GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);
		GRAPHICS_GotoXY(58,0);
		Date_TypeDef sDate = ds3231_read_date();
		sprintf(buffer, "%02u-%02u-%04u", sDate.day, sDate.month, sDate.year);
		GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);

		//draw_box_week(12);

	   // am2320_GetTemperatureAndHumidity(&Am2320_, &temperature, &humidity);
	    GRAPHICS_GotoXY(0,13);
	    //float_to_string(temperature, buffer);
	    GRAPHICS_Puts("Temperature ", &Font_10x10, GRAPHICS_COLOR_WHITE);
	  //  GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);
	    GRAPHICS_GotoXY(0,26);
	    //float_to_string(humidity, buffer);
	    GRAPHICS_Puts("Humidity    ", &Font_10x10, GRAPHICS_COLOR_WHITE);
	   // GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);
	    GRAPHICS_Puts("%", &Font_10x10, GRAPHICS_COLOR_WHITE);


	    // ADC
	  /*  HAL_ADC_Start(&hadc1); // запускаем преобразование сигнала АЦП
	    HAL_ADC_PollForConversion(&hadc1, 100); // ожидаем окончания преобразования
	    uint16_t adc = HAL_ADC_GetValue(&hadc1); // читаем полученное значение в переменную adc
	    HAL_ADC_Start(&hadc1); // запускаем преобразование сигнала АЦП
	    HAL_ADC_PollForConversion(&hadc1, 100); // ожидаем окончания преобразования
	    uint16_t adc2 = HAL_ADC_GetValue(&hadc1); // читаем полученное значение в переменную adc
	    HAL_ADC_Stop(&hadc1);

	   // HAL_ADC_Stop(&hadc1); // останавливаем АЦП (не обязательно)
	    GRAPHICS_GotoXY(0,38);
	    GRAPHICS_Puts("Bat        ", &Font_10x10, GRAPHICS_COLOR_WHITE); // 4095 Это 3.3в
	    float volt = ((3.3 / (float)4095) * (float)adc ); // получаем напряжения на ADC, надо еще добавить делитель
	    volt = volt * 5.02040816; // К - резестивного делителя на входе ADC
	    float_to_string(volt, buffer, 1);
	    buffer[4] = '\0'; // рграничиваем число в три знака
	    GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);
	    GRAPHICS_Puts("V", &Font_10x10, GRAPHICS_COLOR_WHITE);

	    GRAPHICS_GotoXY(0,48);
	    GRAPHICS_Puts("Camera     ", &Font_10x10, GRAPHICS_COLOR_WHITE); // 4095 Это 3.3в
	    volt = ((3.3 / (float)4095) * (float)adc2 ); // получаем напряжения на ADC, надо еще добавить делитель
	    volt = volt * 5.02040816; // К - резестивного делителя на входе ADC
	    float_to_string(volt, buffer, 1);
	    buffer[4] = '\0'; // рграничиваем число в три знака
	    GRAPHICS_Puts(buffer, &Font_10x10, GRAPHICS_COLOR_WHITE);
	    GRAPHICS_Puts("V", &Font_10x10, GRAPHICS_COLOR_WHITE);

		//pressButton_TypeDef statusButton = readButtonStatus(&button_encoder);
	    //uint8_t enc = encoderRead_limit(1);
*/
	    GRAPHICS_UpdateScreen();
	   // HAL_Delay(50);

	}
}

void draw_box_week(uint8_t y, uint8_t action_week)
{

	char masWeek[7][3] = {"Mo\0", "Tu\0", "We\0", "Th\0", "Fr\0", "Sa\0", "Su\0"};
	// x, y, - верхний левый угол
	// w, h - длинна, высота вниз
	uint8_t step_separator = 18;
	uint8_t hight = 13;
	uint8_t hight_marker = 4;
	GRAPHICS_DrawRectangle(0, y , 126 , hight, GRAPHICS_COLOR_WHITE);

	for(int i =0; i<7; i++){
		GRAPHICS_DrawLine(step_separator * (i + 1), y, step_separator * (i + 1), y + hight + hight_marker, GRAPHICS_COLOR_WHITE); // перегородки
		GRAPHICS_GotoXY(5 + (step_separator * i), y + 2);
		GRAPHICS_Puts(masWeek[i], &Font_5x9, GRAPHICS_COLOR_WHITE);
		//GRAPHICS_Puts(masWeek[0], &Font_5x9, GRAPHICS_COLOR_WHITE);
	}

	// отображаем текущий день недели, подсвечивая квадратиком ячейку
	Date_TypeDef sDate = ds3231_read_date();
	uint8_t week = ZellersCongruence(sDate);
	GRAPHICS_DrawFilledRectangle(step_separator * week, y+1 ,step_separator , hight -2, GRAPHICS_COLOR_INVERT);

	// блок отображения маркеров на недели
	GRAPHICS_DrawRectangle(0, y + hight , 126 , hight_marker, GRAPHICS_COLOR_WHITE);

//	struct param settings;
 //  	int err = settings_read(&settings);
//	if(err == -1)printf("ошибка чтения файла настроек");

	for(int i =0; i< 7; i++){ // отрисовываем наш регистр настроек дней недели
		if(action_week & (1 << i))
			GRAPHICS_DrawFilledRectangle((step_separator * i) + 2, y + hight +2 ,step_separator - 4 , hight_marker - 3, GRAPHICS_COLOR_WHITE);
	}
	/*
	GRAPHICS_DrawFilledRectangle((step_separator * 0) + 2, y + hight + 2 ,step_separator - 4 , hight_marker - 3, GRAPHICS_COLOR_WHITE);
	GRAPHICS_DrawFilledRectangle((step_separator * 1) + 2, y + hight +2,step_separator - 4 , hight_marker - 3, GRAPHICS_COLOR_WHITE);
	GRAPHICS_DrawFilledRectangle((step_separator * 2) + 2, y + hight +2 ,step_separator - 4 , hight_marker - 3, GRAPHICS_COLOR_WHITE);
	GRAPHICS_DrawFilledRectangle((step_separator * 3) + 2, y + hight +2 ,step_separator - 4 , hight_marker - 3, GRAPHICS_COLOR_WHITE);
	GRAPHICS_DrawFilledRectangle((step_separator * 4) + 2, y + hight +2 ,step_separator - 4 , hight_marker - 3, GRAPHICS_COLOR_WHITE);
	*/
/*	uint8_t step = 1; // Другой тип выбора ячейки маркера
	GRAPHICS_DrawLine((step_separator * step) + 2, y + hight + 2 , (step_separator * step) + step_separator - 2 , y + hight + 2, GRAPHICS_COLOR_WHITE);
	*/
}

void screen_set_week(void)
{

	uint8_t y = 30;


//	struct param settings;
  // 	int err = settings_read(&settings);
//	if(err == -1)printf("ошибка чтения файла настроек, screen_set_week();");

	uint8_t action_bit_week = settings.action_week;

	pressButton_TypeDef statusButton;
	while(statusButton != PRESS_LONG)
	{
		statusButton = readButtonStatus(&button_encoder);
		GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);

		GRAPHICS_centerLine_Puts(5,"SET ACTION WEEK", &Font_10x10, GRAPHICS_COLOR_WHITE);

		char masWeek[7][3] = {"Mo\0", "To\0", "We\0", "Th\0", "Fr\0", "Sa\0", "Su\0"};
		// x, y, - верхний левый угол
		// w, h - длинна, высота вниз
		uint8_t step_separator = 18;
		uint8_t hight = 13;
		uint8_t hight_marker = 11;
		GRAPHICS_DrawRectangle(0, y , 126 , hight, GRAPHICS_COLOR_WHITE);

		for(int i =0; i<7; i++){
			GRAPHICS_DrawLine(step_separator * (i + 1), y, step_separator * (i + 1), y + hight + hight_marker, GRAPHICS_COLOR_WHITE); // перегородки
			GRAPHICS_GotoXY(5 + (step_separator * i), y + 2);
			GRAPHICS_Puts(masWeek[i], &Font_5x9, GRAPHICS_COLOR_WHITE);
			//GRAPHICS_Puts(masWeek[0], &Font_5x9, GRAPHICS_COLOR_WHITE);
		}

		// отображаем текущий день недели, подсвечивая квадратиком ячейку
		//Date_TypeDef sDate = get_date();
		//uint8_t week = ZellersCongruence(sDate);
		//GRAPHICS_DrawFilledRectangle(step_separator * week, y+1 ,step_separator , hight -2, GRAPHICS_COLOR_INVERT);

		//uint8_t enc = encoderRead_limit(7);
		uint8_t week = encoderRead_limit(0, 6);
		//подсвечиваем выбранный день в верхнем блоке
		GRAPHICS_DrawFilledRectangle(step_separator * week, y+1 ,step_separator , hight -2, GRAPHICS_COLOR_INVERT);
		// нижний блок отображения маркеров на недели
		GRAPHICS_DrawRectangle(0, y + hight , 126 , hight_marker, GRAPHICS_COLOR_WHITE); // рамка блока
		if(statusButton == PRESS_FAST) action_bit_week ^= (1 << week);//инвертируем выбранный день недели

		for(int i =0; i< 7; i++){ // отрисовываем наш регистр настроек дней недели
			if(action_bit_week & (1 << i))
				GRAPHICS_DrawFilledRectangle((step_separator * i) + 2, y + hight +2 ,step_separator - 4 , hight_marker - 4, GRAPHICS_COLOR_WHITE);
		}

		 GRAPHICS_UpdateScreen();
	}
	settings.action_week = action_bit_week;
	settings_write(&settings);
}

void screen_camera_power_on(void)
{
	pressButton_TypeDef statusButton = PRESS_NO; // состояния кнопки энкодера
	set_power_camera(1);
	GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
    GRAPHICS_centerLine_Puts(30, "CAMERA ON", &Font_10x10, GRAPHICS_COLOR_WHITE);
    GRAPHICS_UpdateScreen();

    while(statusButton == PRESS_NO ){
	    statusButton = readButtonStatus(&button_encoder);
	}
//    set_status_system(STAT_STOP);
    set_power_camera(1);
}


