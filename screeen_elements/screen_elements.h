/*
 * screen_elements.h
 *
 *  Created on: 23 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef SCREEEN_ELEMENTS_SCREEN_ELEMENTS_H_
#define SCREEEN_ELEMENTS_SCREEN_ELEMENTS_H_

#include "../SCREEN_GRAPHICS/screen_graphics.h"
#include "../BUTTON/button.h"  // драйвер кнопки
#include "../encoder.h"
#include "../MENY/parametr.h"// тут лежат наши параметры для их настройки через меню
#include <stdint.h>
#include "../MENY/meny.h"
#include "../MENY/module_date_time/date_time_typedef.h"
//#include "am2320.h"
#include "../UTILITY/utility.h"


void screen_main(struct param* settings);
void screen_test_system(void);
void screen_sensor(void);
void draw_box_week(uint8_t y, uint8_t action_week); // draw line box week
void screen_set_week(void);
void screen_camera_power_on(void);


#endif /* SCREEEN_ELEMENTS_SCREEN_ELEMENTS_H_ */
