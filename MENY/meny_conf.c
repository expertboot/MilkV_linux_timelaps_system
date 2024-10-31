
//#include "meny.h"
#include "menu_conf.h"

//struct param parametr;

// ****************************** Статическое конфигурирование меню *****************************
/*
 * Обьявляем переменные которые будем редактироваться через меню, например будут все в этой структуре
 * В итоге их указатели запишем в конфиг меню.
 * struct param
 * {
 *   int16_t         intValue;     // табличный тип t_int
 *   float           floatValue;   // табличный тип t_float
 *   uint8_t         textValue;    // табличный тип t_text_list
 *   RTC_TimeTypeDef time;         // табличный тип t_time
 *   Date_TypeDef    date;         // табличный тип t_date
 * };
 *
 * struct param parametr;         // создаем структуру
 *
 * Далее нам нужно обьявить диапазон переменным, у которых он есть
 *   static int16_t rangeInt[2] = {-20, 100};
 *   static float   rangefloat[2] = {0.1, 10.35};
 *   static uint8_t sizeText = 3;                   // количество строк у параметра textValue, тоесть параметр может меняеться - 0, 1, 2
 *
 * Для текстового параметра нам нужен список с текстом, который будет отображаться в меню
 *   static char *textList[3] = {"kt-315", "STM32", "Buba"};
 *   static uint8_t sizeList = (sizeof(textList) / sizeof(textList[0])) - 1; // вычисляем размер масива строк. Вместо этого Можно указать явно = 3
 *
 *   Для примера у нас есть функция, которую хотим запустить по нажатию в меню, это может быть под меню или все что угодно.
 *   void func_drawSensor(void); // например она отображает данные сенсора на экране
 *
 * Теперь в таблицу конфига записываем указатели на наши данные
 *  line_meny m_line[]= {
 *  	{"nameInt",   "nameInt_2",  &parametr.intValue,   &rangeInt[0],   &rangeInt[1],   t_int,        NONE,   NONE},
 *  	{"nameFloat", "nameFloat2", &parametr.floatValue, &rangefloat[0], &rangefloat[1], t_float,      NONE,   NONE},
 *  	{"nameText",  "nameText2",  &parametr.textValue,  NONE,           &sizeList,      t_text_list,  NONE,   textList},
 *  	{"nameTime",  "nameTime2",  &parametr.time,       NONE,           NONE,           t_time,       NONE,   NONE},
 *  	{"nameDate",  "nameDate2",  &parametr.date,       NONE,           NONE,           t_date,       NONE,   NONE},
 *  	{"nameFunc",  "nameFunc2",  func_drawSensor,      NONE,           NONE,           t_function,   NONE,   NONE},
 *  	{" EXIT",     NONE,         NONE,                 NONE,           NONE,           t_exit,       NONE,   NONE},  // выход из меню
 *  };      |           |             |                     |               |                |            |       |
 *          |           |             |                     |               |                |            |       Указатель на список строк для t_text_list
 *          |           |             |                     |               |                |            Функция, запускаеться после выхода из редактирования
 *          |           |             |                     |               |                Тип данных TypeValue_TypeDef
 *          |           |             |                     |               Указатель на диапазон макс, если это t_text_list то это количество строк в списке
 *          |           |             |                     Указатель на диапазон переменной минимум
 *          |           |             Указатель на переменную, которую будем редактировать в меню
 *          |           Текст который будет показываться в подменю настройки переменной
 *          Текст который отображаеться в главном меню
 */
/*
	 line_meny m_line[]=
	 {
		 {" BEGIN   ",    "BEGIN TIME",  &parametr.time_start,     NONE,                     NONE,            t_time,     bkp_set_timeStart},
		 {" END     ",    "END TIME",    &parametr.time_stop,      NONE,                     NONE,            t_time,     bkp_set_timeStop},
		 {" INTERVAL",    "INTERVAL",    &parametr.time_interval,  NONE,                     NONE,            t_time,     bkp_set_timeInterval},
		 {" TIME    ",    "TIME RTC",    &parametr.time_rtc,       NONE,                     NONE,            t_time,     ds3231_write_rtc },
		 {" DATE  ",      "DATE RTC",    &parametr.date_rtc,       NONE,                     NONE,            t_date,     ds3231_write_date },
		 {" WEEK ACTION",  " ",          screen_set_week,          NONE,                     NONE,            t_function,  NONE},
		 {" - SENSOR",  " ",             screen_sensor,            NONE,                     NONE,            t_function,  NONE},
		 {" - RP POWER ON",    " ",      screen_rp_power_on,       NONE,                     NONE,            t_function,  NONE},
		 {" - RP POWER OFF",    " ",     screen_rp_soft_off,       NONE,                     NONE,            t_function,  NONE},
		 {" - CAMERA ON",  " ",          screen_camera_power_on,   NONE,                     NONE,            t_function,  NONE},
		 {" - TEST SYSTEM",    " ",      screen_test_system,       NONE,                     NONE,            t_function,  NONE},
		 {" EXIT",        "",            NONE,                     NONE,                     NONE,            t_exit,      NONE },
	 };
*/

/*
	config_meny conf_meny_base = // конфигурация базового меню
	 {
			m_line,              // указатель на масив строчек меню
			PIXEL_SPICE,         // pixelSpace       // промежуток между строками в пикселях
			 (sizeof(m_line) / sizeof(m_line[0])) - 1,  // количество строк в меню, высчитываем размер
			 SIZE_SHOW_LINE - 1, // sizeShow_line    // количество отображаемых строк - 3 шт (0,1,2)
			 WIDTH_PIX,          // backlight_length // длина подсветки линии меню в пикселях
			 ENABLE_LOOP,        // loop_menu        // зациклить меню, для выхода надо удерживать энкодер, если 0 то меню не блокирующее
			0,                   // x_pix; // кордината начало меню по горизонту, слево на право
			0,                   // y_pix; // кордината начала меню по вертикали, сверху вниз
			WIDTH_PIX + 1,       // w_pix; // ширина меню по горизонту вправо
			(PIXEL_SPICE * SIZE_SHOW_LINE) + 3,// h_pix; // высота меню по вертикали вниз
			ENABLE_FRAME,
			&Font_10x10,         // шрифт главного меню
			&Font_19x18,
	 };

*/
