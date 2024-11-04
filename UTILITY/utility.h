/*
 * utility.h
 *
 *  Created on: Sep 9, 2024
 *      Author: ro_rudenko
 */

#ifndef __UTILITY_H
#define __UTILITY_H

#include <stdint.h>
#include "../DS3231/ds3231.h"
//#include "am2320.h"
#include "../time_calc/time_calc.h"
#include "../SETTINGS/settings.h"
#include <stdio.h>


typedef enum{
	RP_MODE_SETUP,       // режим настройки RP, она не включиться и не будет делать снимок
	RP_MODE_RUN,         // режим работы, при включении она сделает снимок и вырубиться
//	RP_MODE_TEST_SYSTEM, // тестовый запуск RP и проверка камеры
}rp_mode_TypeDef;

void debug_string(const char* string);

// read/write DS3231 RTC
void ds3231_write_rtc(RTC_TimeTypeDef sTime);
RTC_TimeTypeDef ds3231_read_rtc(void);
void ds3231_write_date(Date_TypeDef sDate );
Date_TypeDef ds3231_read_date(void);
void ds3231_write_alarm(RTC_TimeTypeDef timeAlarm);

void set_power_camera(uint8_t state);

// Сalculation of the next active time
void calculate_next_time(RTC_TimeTypeDef *time, Date_TypeDef* date);
Date_TypeDef next_action_date(Date_TypeDef sDate_curent);


#endif /* UTILITY_H_ */
