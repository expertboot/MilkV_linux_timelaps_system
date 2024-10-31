
#ifndef MENU_H
#define MENU_H


#include "parametr.h"        // тут лежат переменные, их меняем через меню
#include <stdint.h>
#include <math.h>
#include "menu_conf.h"
#include "module_date_time/date_time_typedef.h"

#include "../BUTTON/button.h"          // драйвер кнопки
#include "../encoder.h"         // драйвер энкодера, его две функции указаны в menu.c
#include "../SCREEN_GRAPHICS/screen_graphics.h" // драйвер графики

// ПОДКЛЮЧАЕМ НУЖНЫЕ МОДУЛИ ПЕРЕМЕННЫх В МЕНЮ
#define MODULE_INT
//#define MODULE_FLOAT
#define MODULE_TEXTLIST
#define MODULE_DATE_TIME

//#define MODE_ONE_BUTTON // в этом режиме энкодер отключаеться, все управления происходит по одной кнопки

#ifdef MODULE_INT
#include "module_int/module_int.h"
#endif
#ifdef MODULE_FLOAT
#include "module_float/module_float.h"
#endif
#ifdef MODULE_TEXTLIST
#include "module_text/module_text.h"
#endif
#ifdef MODULE_DATE_TIME
#include "module_date_time/module_date_time.h"
#endif

// внешнии функции который запускаються из меню
// EXTERNAL FUNCTIONS FOR LAUNCHING FROM THE MENU
extern void start_run(void);
extern void screen_rp_power_on(void);
extern void screen_camera_power_on(void);
extern void sensor_screen(void);
extern void screen_set_week(void);
extern void screen_sensor(void);
extern void screen_test_system(void);
extern void screen_rp_soft_off(void);
// post function after parameter change (save param func)
/*extern void ds3231_write_rtc(RTC_TimeTypeDef sTime);
extern void bkp_set_timeInterval(RTC_TimeTypeDef sTime);
extern void bkp_set_timeStart(RTC_TimeTypeDef sTime);
extern void bkp_set_timeStop(RTC_TimeTypeDef sTime);
extern RTC_TimeTypeDef ds3231_read_rtc(void);
extern void ds3231_write_date(Date_TypeDef sDate);
*/

// ********************************** Basic Functional Menu ******************************************
void menu_init(config_meny *conf_meny_base, line_meny *lineMeny);
void menu_instal_Button(config_meny *conf_meny_base, button_TypeDef *button_encoder); // Нужна при статической конфигурации меню
//void meny_instal_Encoder(config_meny *conf_meny_base, int* count_encoder);

uint8_t add_lineMenu_func(config_meny *config, char* name, void* function);
uint8_t add_lineMenu_exit(config_meny *config, char* name);

void menu_draw(const config_meny *conf_meny); // The main function of start the menu
void draw_title(const config_meny *conf_meny, uint8_t numline);
void draw_topRectangle(unsigned char border);
void erase_topRectangle(unsigned char border);
void draw_lineMenyOne(const config_meny *conf_meny,uint8_t numStroke, uint8_t numline, uint8_t maxSizePix);
void value_convert_changeStrok(void* value, TypeValue_TypeDef typeValue, char* buffer); // master func convert value to mrny string

// функции одновременно возвращают состояния энкодера и кнопки
int16_t menu_readControls(pressButton_TypeDef *statusButton);
void menu_writeControls(int16_t count);
int16_t menu_сontrols_limit(int16_t min, int16_t max, pressButton_TypeDef *statusButton);

#endif
