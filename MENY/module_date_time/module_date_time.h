/*
 * module_date_time.h
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef MENY_MODULE_DATA_TIME_MODULE_DATE_TIME_H_
#define MENY_MODULE_DATA_TIME_MODULE_DATE_TIME_H_

#include "../menu_conf.h"
#include "../../BUTTON/button.h"
//#include "meny.h"
#include "date_time_typedef.h"

extern button_TypeDef *menu_button_enter;

void add_lineMenu_time(config_meny *config,  char* name1, char* name2, RTC_TimeTypeDef* value, void* postFunc);
void add_lineMenu_date(config_meny *config,  char* name1, char* name2, Date_TypeDef* value, void* postFunc);

void time_to_string(RTC_TimeTypeDef btime, char *buf);
void date_to_string(Date_TypeDef sDate, char *buf);

RTC_TimeTypeDef stroke_to_time(char* buffer);
Date_TypeDef stroke_to_date(char* buffer);

uint8_t time_convert_changeStrok(const config_meny *conf_meny, uint8_t numStroke, char* ext_buffer);
uint8_t date_convert_changeStrok(const config_meny *conf_meny, uint8_t numStroke, char* ext_buffer);
void change_number_time_date(const config_meny *conf_meny, char* buffer, uint16_t x, uint16_t y, uint8_t numline);
void top_meny_change_time_date(const config_meny *conf_meny, uint8_t numline);

#endif /* MENY_MODULE_DATA_TIME_MODULE_DATE_TIME_H_ */
