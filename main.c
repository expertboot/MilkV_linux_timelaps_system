#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
//#include <string.h>
#include <signal.h>

#include "MENY/module_date_time/date_time_typedef.h"
#include "OLED_I2C/ssd1306_i2c.h"
#include "SCREEN_GRAPHICS/fonts.h"
#include "SCREEN_GRAPHICS/screen_graphics.h"
#include "SETTINGS/settings.h"
#include "wiringx.h"
#include <pthread.h>
#include <sys/syscall.h>

#include "encoder.h"
#include "BUTTON/button.h"
#include "MENY/meny.h"
#include "DS3231/ds3231.h"
#include "UTILITY/utility.h"
#include "screeen_elements/screen_elements.h"


volatile int running = 1; 
button_TypeDef button_encoder;

struct param settings;
RTC_TimeTypeDef rtc_time_current;
Date_TypeDef rtc_date_current;

line_meny arr_line[15]; // масив линий, тут храним все линии меню
config_meny config;    // главный конфиг меню

void handle_sigint(int sig) {
    running = 0; // Устанавливаем флаг для завершения потоков

    printf("END MAIN FUNC..\n");
    GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
    GRAPHICS_UpdateScreen();
    encoderClose();
    wiringXGC();
}

void funcMenu_saveSettings(void)
{
	if(settings_write(&settings) == -1)
	{
		printf("ошибка записи настроек, funcMenu_saveSettings()");
	}
}

int main()
{
	signal(SIGINT, handle_sigint);

    if(wiringXSetup("milkv_duo", NULL) == -1) {
        wiringXGC();
        return -1;
    }

	if(SSD1306_Init() == 0)
	{
		printf("OLED NONE\n");
		return -1;
	}

	
	printf("Start programm ROOD \n");
	
	button_encoder.flagInv = BUTTON_INV;
	button_encoder.pin = 8;
	buttonInit(&button_encoder);


	GRAPHICS_Init();
	GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
   
    GRAPHICS_GotoXY(0,0);	
	GRAPHICS_Puts("Hello ROOD  MILK-V", &Font_10x10, GRAPHICS_COLOR_WHITE);
	GRAPHICS_UpdateScreen();
	sleep(1);

    encoderInit(1,0);

	DS3231_Init( "/dev/i2c-1");
	rtc_time_current = ds3231_read_rtc();
	rtc_date_current = ds3231_read_date();

	int err_settings;
	err_settings = settings_read(&settings);
	if(err_settings == -1)
	{
//		printf("Ошибка чтения файла настроек\n");	
		printf("Запись дефолтных настроек\n");
		settings.action_week = 0b00110011;
		settings.time_interval.Seconds = 5;
		settings.time_start.Seconds = 0;
		settings.time_start.Minutes = 10;	
        settings.time_start.Hours = 0;
		settings.time_stop.Seconds = 13;
		settings_write(&settings);
	}
	// *********************** Динамическое создания моню *********************************

	menu_init(&config, arr_line);                  // инциализируем главный конфиг с гарф.настройками
	menu_instal_Button(&config,  &button_encoder); // передаем конпку которая будет рулить меню
	// Формируем само меню
	add_lineMenu_time(&config, " BEGIN   ", "BEGIN TIME",  &settings.time_start, funcMenu_saveSettings);
	add_lineMenu_time(&config, " END     ", "END TIME", &settings.time_stop, funcMenu_saveSettings);
	add_lineMenu_time(&config, " INTERVAL", "INTERVAL", &settings.time_interval, funcMenu_saveSettings);
	add_lineMenu_time(&config, " TIME    ", "TIME RTC", &settings.time_rtc, ds3231_write_rtc);
	add_lineMenu_date(&config, " DATE    ", "DATE RTC", &settings.date_rtc, ds3231_write_date);
	add_lineMenu_func(&config, " WEEK ACTION", screen_set_week);
	add_lineMenu_func(&config, " - SENSOR", screen_sensor);
	add_lineMenu_func(&config, " - CAMERA ON", screen_camera_power_on);
	add_lineMenu_func(&config, " - TEST SYSTEM", screen_test_system);
	add_lineMenu_exit(&config, "  EXIT");

//	menu_draw(&config);
	screen_main(&settings);


/*	while(running)
	{
		sprintf(str_isr, "%i", encoderRead());  
	//	sprintf(str, "%u", second);  
		GRAPHICS_GotoXY(0,20);
		GRAPHICS_Puts(str, &Font_10x10,GRAPHICS_COLOR_WHITE);
		GRAPHICS_Puts("  ", &Font_19x18,GRAPHICS_COLOR_WHITE);
		GRAPHICS_Puts(str_isr, &Font_10x10,GRAPHICS_COLOR_WHITE);
		GRAPHICS_UpdateScreen();
	
	};
*/
	handle_sigint(0);
}
