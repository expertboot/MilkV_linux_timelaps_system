#include "meny.h"
#include <stdint.h>

//extern struct param parametr;
button_TypeDef *menu_button_enter;

#ifdef MODE_ONE_BUTTON
static int16_t count_button = 0;
#endif

#include "../UTILITY/utility.h"

extern struct param settings;

// Блок управления меню, тут реализуем логику управления по энкодеру или по кнопкам
int16_t menu_readControls(pressButton_TypeDef *statusButton)
{
   *statusButton = readButtonStatus(menu_button_enter);

#ifdef MODE_ONE_BUTTON
	if(*statusButton == PRESS_FAST){
		count_button++;
		*statusButton = PRESS_NO;
	} else {
		if(*statusButton == PRESS_LONG)
		{
			*statusButton = PRESS_FAST;
		}
	}
	return count_button;
#else
	return encoderRead();
#endif
}


void menu_writeControls(int16_t count)
{
#ifdef MODE_ONE_BUTTON
	count_button = count;
#else
	encoderWrite(count);
#endif
}
// ***************************************************************************

int16_t menu_сontrols_limit(int16_t min, int16_t max, pressButton_TypeDef *statusButton)
{
	int16_t encoder = menu_readControls(statusButton);

#ifdef MODE_ONE_BUTTON // если режим одной кнопки, то счетчиком ходим по кругу
	if(encoder > max){ menu_writeControls(min); encoder = min;}
	if(encoder < min){ menu_writeControls(max); encoder = max;}
#else
	if(encoder > max){ menu_writeControls(max); encoder = max;}
	if(encoder < min){ menu_writeControls(min); encoder = min;}
#endif
	return encoder;
}


void menu_init(config_meny *conf_meny_base, line_meny *lineMeny)
{
	conf_meny_base->lineMeny = lineMeny;
	conf_meny_base->pixelSpace = PIXEL_SPICE;// промежуток между строками в пикселях
	conf_meny_base->x_pix = 0; // кордината начало меню по горизонту, слево на право
	conf_meny_base->y_pix = 0; // кордината начала меню по вертикали, сверху вниз
	conf_meny_base->w_pix = WIDTH_PIX + 1; // длинна меню по горизонту вправо
	conf_meny_base->h_pix = (PIXEL_SPICE * SIZE_SHOW_LINE) + 3; // высота меню по вертикали вниз
	conf_meny_base->sizeLine = 0; // количество строк в меню, счетчик добавленных строк
	conf_meny_base->sizeShow_line = SIZE_SHOW_LINE - 1;// количество отображаемых строк - 3 шт (0,1,2)
	conf_meny_base->backlight_length = WIDTH_PIX; // Длина подсветки линии меню в пикселях
	conf_meny_base->enable_frame = ENABLE_FRAME; // включить рамку вокруг меню
	conf_meny_base->enable_loop = ENABLE_LOOP;   // зациклить меню, для выхода надо удерживать энкодер
	conf_meny_base->Font = &Font_10x10;  // шрифт главного меню
	conf_meny_base->FontBig = &Font_19x18;// большой шрифт для настройки параметров
}

uint8_t add_lineMenu_func(config_meny *config, char* name, void* function)
{
	strcpy(config->lineMeny[config->sizeLine].name_1, name);
	config->lineMeny[config->sizeLine].value = function;
	config->lineMeny[config->sizeLine].minValue = NONE;
	config->lineMeny[config->sizeLine].maxValue = NONE;
	config->lineMeny[config->sizeLine].post_func = NONE;
	config->lineMeny[config->sizeLine].typeValue = t_function;
	config->sizeLine++;
	return 0;
}

uint8_t add_lineMenu_exit(config_meny *config, char* name)
{
	strcpy(config->lineMeny[config->sizeLine].name_1, name);
	config->lineMeny[config->sizeLine].value = NONE;
	config->lineMeny[config->sizeLine].minValue = NONE;
	config->lineMeny[config->sizeLine].maxValue = NONE;
	config->lineMeny[config->sizeLine].post_func = NONE;
	config->lineMeny[config->sizeLine].typeValue = t_exit;
	config->sizeLine++;
	return 0;
}

void menu_instal_Button(config_meny *conf_meny_base, button_TypeDef *button)
{
	menu_button_enter = button;
}

void value_convert_changeStrok(void* value, TypeValue_TypeDef typeValue, char* buffer)
{
 	 switch((uint8_t)typeValue)
 	 {
#ifdef MODULE_FLOAT
	 case t_float:      float_to_string(*((float*)value) ,buffer, 1);          break;
#endif
#ifdef MODULE_INT
		 case t_int:    sprintf(buffer, "%i",    *((int16_t*)value));       break;
#endif
#ifdef MODULE_DATE_TIME
		 case t_time:   time_to_string(*((RTC_TimeTypeDef*)value) ,buffer); break;
		 case t_date:   date_to_string(*((Date_TypeDef*)value) ,buffer);    break;
#endif
 	 }
}

// рисуем одну строку меню на указанной линии, учитываем размер меню, если надпись выходит за пределы меню то обрезаем её
void draw_lineMenyOne(const config_meny *conf_meny, uint8_t numStroke, uint8_t numline, uint8_t maxSizePix)
{
	 char buffer[MAX_SYMBOL_BUF];
	 int16_t pixLimit = conf_meny->w_pix;
	 FontDef_t *Font = conf_meny->Font;
	// рисуем короткое название параметра
	 GRAPHICS_GotoXY(2 + conf_meny->x_pix, conf_meny->y_pix + ((conf_meny->pixelSpace + Font->FontHeight) * numline) + 3);
	 GRAPHICS_PutsToLimit((char*)conf_meny->lineMeny[numStroke].name_1, pixLimit, Font, GRAPHICS_COLOR_WHITE);

	 pixLimit = pixLimit - GRAPHICS_lengtchPixStroke((char*)conf_meny->lineMeny[numStroke].name_1, Font);

	 // костыль для выравнивания параметров
	 if(maxSizePix != 0) GRAPHICS_GotoXY(2 + conf_meny->x_pix + maxSizePix, conf_meny->y_pix + ((conf_meny->pixelSpace + Font->FontHeight) * numline) + 3);


	 if(pixLimit < Font->FontWidth) return;
     // после чего распознаем тип параметра и выводим его следом за названием параметра
	 if(conf_meny->lineMeny[numStroke].typeValue == t_text_list){
  		 uint8_t value = *((uint8_t*)conf_meny->lineMeny[numStroke].value);
  		 GRAPHICS_PutsToLimit((char*)conf_meny->lineMeny[numStroke].list_param[value], pixLimit, Font, GRAPHICS_COLOR_WHITE);
  		 pixLimit = pixLimit - GRAPHICS_lengtchPixStroke((char*)conf_meny->lineMeny[numStroke].name_1, Font);
         return;
	 }
	 if(conf_meny->lineMeny[numStroke].typeValue == t_function) return; // параметра для функции нету, просто игнорим
	 if(conf_meny->lineMeny[numStroke].typeValue == t_exit)     return; // параметра команды выхода, просто игнорим

	 value_convert_changeStrok((char*)conf_meny->lineMeny[numStroke].value, conf_meny->lineMeny[numStroke].typeValue, buffer);
	 GRAPHICS_PutsToLimit(buffer, pixLimit, Font,  GRAPHICS_COLOR_WHITE);
}

// рисует заголовок по центру верха рамки
void draw_title(const config_meny *conf_meny, uint8_t numline)
{
	// очищаем место под надпись заголовка
	uint8_t pixLen  = GRAPHICS_lengtchPixStroke((char*)conf_meny->lineMeny[numline].name_2, conf_meny->Font); // узнаем длинну строки
	uint8_t pix_x = (GRAPHICS_WIDTH - pixLen) / 2;
	GRAPHICS_DrawFilledRectangle(pix_x-2, BORDER_TOP_WINDOWS - (conf_meny->Font->FontHeight/2) - 2, pixLen +1, conf_meny->Font->FontHeight + 2, GRAPHICS_COLOR_BLACK);
	// пишем заголовок подменю, название распологаем по цент
	GRAPHICS_centerLine_Puts(BORDER_TOP_WINDOWS - (conf_meny->Font->FontHeight/2), (char*)conf_meny->lineMeny[numline].name_2, conf_meny->Font, GRAPHICS_COLOR_WHITE);
}

// запуск меню из конфигурации
void menu_draw(const config_meny *conf_meny)
{
	static int8_t   lineBacklight = 0; // номер подсвеченной строки
	static uint8_t  offset = 0;        // смещение экрана
	static uint8_t  numline;
	static int16_t  encoder = 0;
	       int16_t  saveEncoder = 0xFFAA;

	pressButton_TypeDef statusButton;

	if(conf_meny->sizeLine == 0) return;
	uint8_t sizeLine = conf_meny->sizeLine - 1;


// ******** RTC костыль *************
uint8_t saveSecondRTC = 0;
//RTC_TimeTypeDef rtc_time_current;


// ******************************

	menu_writeControls(0);


    while(1)
    {
// ******** RTC костыль *************
    	settings.time_rtc = ds3231_read_rtc();// костыль, чиьаем время для его показа в меню как оно тикает
// ******************************

     // очистка области отрисовки меню
     GRAPHICS_DrawFilledRectangle(conf_meny->x_pix, conf_meny->y_pix, conf_meny->w_pix, (conf_meny->pixelSpace + conf_meny->Font->FontHeight)* (conf_meny->sizeShow_line +1) + 1, GRAPHICS_COLOR_BLACK); // GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
     if(conf_meny->enable_frame) GRAPHICS_DrawRectangle(conf_meny->x_pix, conf_meny->y_pix, conf_meny->w_pix, (conf_meny->pixelSpace + conf_meny->Font->FontHeight)*( conf_meny->sizeShow_line +1) + 1, GRAPHICS_COLOR_WHITE);

     encoder  = menu_сontrols_limit(0, sizeLine, &statusButton);

	 lineBacklight = encoder - offset; // вычисляем разницу
	 while(lineBacklight > conf_meny->sizeShow_line)
	 {
		 offset++;
		 lineBacklight = encoder - offset;
	 } // если разница больше чем отображае количество строк то сдвигаем список строк вверх
	 while(lineBacklight < 0)
	 {
		 offset--;
		 lineBacklight = encoder - offset;
	 } // если разница меньше то сдвигаем список строк ввниз

	 //вычисляем макисмальную длину имени во всем меню в пикселях
	 uint8_t maxSazePix = 0;
	 for(uint8_t i=0; i <= sizeLine; i++)
	 {
		 if(conf_meny->lineMeny[i].typeValue == t_time){
			uint8_t sizePix =  GRAPHICS_lengtchPixStroke((char*)conf_meny->lineMeny[i].name_1, conf_meny->Font);
			if(maxSazePix < sizePix) maxSazePix = sizePix;
		 }
	 }

	 // отрисовываем строки меню
	 for(uint8_t i=0; i <= sizeLine; i++){
		 numline = i - offset;

		 if(conf_meny->lineMeny[i].typeValue == t_time){ // Для выравнивание параметра время (костыль)
			 if(numline <= conf_meny->sizeShow_line) draw_lineMenyOne(conf_meny, i, numline, maxSazePix);
		 }
		 else if(numline <= conf_meny->sizeShow_line) draw_lineMenyOne(conf_meny, i, numline, 0);
	 }
	 // отрисовываем подсветку выделенной строки
	 GRAPHICS_DrawFilledRectangle(conf_meny->x_pix + 1, conf_meny->y_pix + 1 + ((conf_meny->pixelSpace + conf_meny->Font->FontHeight) * lineBacklight), conf_meny->backlight_length - 1, conf_meny->Font->FontHeight + 2, GRAPHICS_COLOR_INVERT);

	 if(statusButton == PRESS_LONG){  // выход из меню
	     // очистка области отрисовки меню
	     GRAPHICS_DrawFilledRectangle(conf_meny->x_pix, conf_meny->y_pix, conf_meny->w_pix, conf_meny->h_pix, GRAPHICS_COLOR_BLACK); // GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
	     menu_writeControls(0);
	     return;
	 }

     if(conf_meny->enable_loop == 1 ){  // если стоит цикл меню, то обновляем экран
    	 // костыль, для того что бы время шло в меню
 		    if(saveEncoder != encoder || saveSecondRTC != settings.time_rtc.Seconds ) // RTC костыль
 		  //  if(saveEncoder != encoder )
				if(GRAPHICS_isBusy() == 0)
				{
					 GRAPHICS_UpdateScreen();
				//	 printf("timeRTC = %u.\n", settings.time_rtc.Seconds);  
					 saveEncoder = encoder; // если обновления экрана занято предыдущим действияем, то нечего не делаем
					 saveSecondRTC = settings.time_rtc.Seconds;
				}
 	 }

    if(statusButton == PRESS_FAST) // запускаем нужный обработчик параметра
     {
        uint8_t encoderSave = encoder;
    	 switch((uint8_t)conf_meny->lineMeny[encoder].typeValue)
    	 {

			 case t_function: { void (*func_ptr)() = (void (*)())conf_meny->lineMeny[encoder].value; func_ptr();  } break;
#ifdef MODULE_TEXTLIST
			 case t_text_list: top_meny_changeListText(conf_meny, encoder, conf_meny->Font); break;
#endif
#ifdef MODULE_INT
			 case t_int:       top_meny_changeInt(conf_meny, encoder); break;
#endif
#ifdef MODULE_FLOAT
			 case t_float:     top_meny_changeFloat(conf_meny, encoder); break;
#endif
#ifdef MODULE_DATE_TIME
			 case t_time:      top_meny_change_time_date(conf_meny, encoder); break;
			 case t_date:      top_meny_change_time_date(conf_meny, encoder); break;
#endif
			 case t_exit:      GRAPHICS_DrawFilledRectangle(conf_meny->x_pix, conf_meny->y_pix, conf_meny->w_pix, conf_meny->h_pix, GRAPHICS_COLOR_BLACK); return; break;
    	 }
		 statusButton = PRESS_NO;
		 saveEncoder = 0xFFAA;
		 menu_writeControls(encoderSave);
     }

     if(conf_meny->enable_loop == 0) break; // если стоит цикл меню, то обновляем экран, иначе выходим из цикла
    }
}

// очистка только рабочий области поверхностного окна
void erase_topRectangle(unsigned char border)
{
	 GRAPHICS_DrawFilledRectangle(border+1, border+6, GRAPHICS_WIDTH - (border*2)-2, GRAPHICS_HEIGHT - (border*2) - 8, GRAPHICS_COLOR_BLACK);
}
// рисуем квадрат с рамкой по вверх всего
void draw_topRectangle(unsigned char border){ // border - отступ рамки в пикселях
	// -4 это смещение тени под рамкой
	 GRAPHICS_DrawFilledRectangle(border-4, border-4, GRAPHICS_WIDTH - (border*2)+8, GRAPHICS_HEIGHT - (border*2)+8, GRAPHICS_COLOR_BLACK);
	 GRAPHICS_DrawRectangle(border, border, GRAPHICS_WIDTH - (border*2), GRAPHICS_HEIGHT - (border*2), GRAPHICS_COLOR_WHITE); // первая рамка
	// GRAPHICS_DrawFilledRectangle(border+1,border+1, GRAPHICS_WIDTH - (border*2)-2, GRAPHICS_HEIGHT - (border*2)-2, GRAPHICS_COLOR_BLACK);
}

// ****************************** Функции создания меню с выделением памяти для переменных ****************************************



