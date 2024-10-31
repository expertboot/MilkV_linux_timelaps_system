#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
//#include <string.h>
#include <signal.h>

#include "OLED_I2C/ssd1306_i2c.h"
#include "SCREEN_GRAPHICS/fonts.h"
#include "SCREEN_GRAPHICS/screen_graphics.h"
#include "wiringx.h"
#include <pthread.h>
#include <sys/syscall.h>

#include "encoder.h"
#include "BUTTON/button.h"
#include "MENY/meny.h"

volatile int running = 1; // Переменная для контроля выполнения
button_TypeDef button_encoder;


void handle_sigint(int sig) {
    running = 0; // Устанавливаем флаг для завершения потоков

    printf("END MAIN FUNC..\n");
    GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
    GRAPHICS_UpdateScreen();
    encoderClose();
    wiringXGC();
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

	char str[20];
	char str_isr[20];

    encoderInit(1,0);
//	pressButton_TypeDef status_button;

	  // *********************** Динамическое создания моню *********************************
  // определяем параметры которые будут изменяться через меню
  struct param{
    int16_t         value_int;   // табличный тип t_int
  //  float           value_float; // табличный тип t_float
    uint8_t         value_text;  // табличный тип t_text_list
    RTC_TimeTypeDef time;        // табличный тип t_time
    Date_TypeDef    date;        // табличный тип t_date
  };

  struct param paramValue;
  char *text_list[5] = {"one" , "two" ,"three" ,"text_4" ,"text_5" };

  line_meny arr_line[7]; // масив линий, тут храним все линии меню
  config_meny config;    // главный конфиг меню

  menu_init(&config, arr_line);                  // инциализируем главный конфиг с гарф.настройками по дефолту
  menu_instal_Button(&config,  &button_encoder); // передаем конпку которая будет рулить меню

  // Формируем само меню
  add_lineMenu_int(&config,   " PARAMETR   ", "Set value",  &paramValue.value_int,    0,    100,   NONE);
  add_lineMenu_text(&config,  " SELECT TYP ", "set select", &paramValue.value_text,  text_list, 5, NONE);
  add_lineMenu_time(&config,  " SET TIME ",     "SET TIME",   &paramValue.time, NONE);
  add_lineMenu_date(&config,  " SET DATE ",    "SET date",   &paramValue.date, NONE);
  add_lineMenu_func(&config,  " START FUNC ",   NONE);
  add_lineMenu_exit(&config,  "  EXIT");


  // Отрисовка меню в режиме зацикливание, режим можно поменять в menu_conf.h
  menu_draw(&config);

	while(running)
	{
	/*	status_button = readButtonStatus(&button_encoder);
		GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
		GRAPHICS_GotoXY(0,0);
		if(status_button == PRESS_FAST)
		//if( digitalRead(pinB) != 0)
		{
			GRAPHICS_Puts("FAST" ,&Font_10x10, GRAPHICS_COLOR_WHITE);	
		    GRAPHICS_UpdateScreen();
			sleep(1);
		}
		if(status_button == PRESS_LONG)
		{
			GRAPHICS_Puts("LONG" ,&Font_10x10, GRAPHICS_COLOR_WHITE);	
		    GRAPHICS_UpdateScreen();
			sleep(1);
		}
		//	if(status_button == PRESS_LONG) printf("press - long");
	*/	
		sprintf(str_isr, "%i", encoderRead());  
	//	sprintf(str, "%u", value);  
		GRAPHICS_GotoXY(0,20);
		//GRAPHICS_Puts(str, &Font_10x10,GRAPHICS_COLOR_WHITE);
		GRAPHICS_Puts("  ", &Font_19x18,GRAPHICS_COLOR_WHITE);
		GRAPHICS_Puts(str_isr, &Font_10x10,GRAPHICS_COLOR_WHITE);
		GRAPHICS_UpdateScreen();
	
	};


}
