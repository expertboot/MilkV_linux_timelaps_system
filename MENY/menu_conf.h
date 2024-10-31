/*
 * menu_conf.h
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef MENY_MENU_CONF_H_
#define MENY_MENU_CONF_H_

//#include "../SCREEN_GRAPHICS/screen_graphics.h" // Graphic driver
#include <stdint.h>
#include "../SCREEN_GRAPHICS/fonts.h"
//#include "button.h"

#define BORDER_TOP_WINDOWS 9 // borders of the top window from the screen size
#define MAX_SYMBOL_BUF 20     // Buffer size for menu name string variables
#define NONE 0

// Default graphical menu config
#define	Y_PIX 0              // Coordinates of the menu start vertically, top to bottom
#define WIDTH_PIX 125        // Menu width in pixels
#define PIXEL_SPICE 6        // line spacing in pixels. default =6
#define SIZE_SHOW_LINE 4     // Number of lines displayed:  4 = (0,1,2,3)
#define BACK_LIGHT_HEIGHT 16 // Menu line highlight height in pixels. default =16
#define ENABLE_LOOP 1        // Loop the menu, to exit you need to hold the encoder, if 0 then the menu is not blocking
#define	ENABLE_FRAME 1       // Enable frame around menu


typedef enum {
   t_int,      // int16
   t_float,    // float
   t_text_list,// Список тикстовых параметров, выборать строку из списка строк. Номер этой строки сохраниться в переменную
   t_function, // Запуск функции
   t_time,     // Настройка времени 00:00:00
   t_date,     // Настройка даты 00-00-00
   t_exit,     // выход из меню
} TypeValue_TypeDef;

//структура одной строчки меню переменной
typedef struct line_meny{
	char name_1[15];              // имя параметра отображаемое в меню
	char name_2[20];              // полное имя параметра отображаемое в окне настроке параметра
	void *value;            // указатель на переменную
	void *minValue;         // минимальное значение переменной
	void *maxValue;         // максимальное значение переменной
	TypeValue_TypeDef typeValue; // тип переменной
	void *post_func;        // пост функция, запустить пссле выха из настройки. В нее передаеться парамтр настройки самой.
	char **list_param;            // список имен параметров для кастомного меню, для других типов не важно
}line_meny;

// структура конфигурации меню, тут все необходимые параметры для отрисовки меню
typedef struct config_meny{
	  line_meny *lineMeny; // масив строк для t_text_list
	  uint8_t pixelSpace;        // промежуток между строками в пикселях
	  uint8_t sizeLine;          // количество строк в меню
	  uint8_t sizeShow_line;     // количество отображаемых строк, видимый участок на экране
	  uint8_t backlight_length;  // длина подсветки линии меню в пикселях
	  uint8_t enable_loop;       // зациклить меню, для выхода надо удерживать энкодер, если 0 то меню не блокирующее
	 // кординаты расположения меню и размер окна для меню
	  uint8_t x_pix;        // кордината начало меню по горизонту, слево на право
	  uint8_t y_pix;        // кордината начала меню по вертикали, сверху вниз
	  uint8_t w_pix;        // длинна меню по горизонту вправо
	  uint8_t h_pix;        // ширина меню по вертикали вниз
	  uint8_t enable_frame; // включить рамку вокруг меню
	  FontDef_t *Font;      // шрифт главного меню
	  FontDef_t *FontBig;   // большой шрифт для редактирования переменной
}config_meny;


extern config_meny conf_meny_base;


#endif /* MENY_MENU_CONF_H_ */
