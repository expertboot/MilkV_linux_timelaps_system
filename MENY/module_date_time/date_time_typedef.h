/*
 * date_time_typedef.h
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef MENY_MODULE_DATE_TIME_DATE_TIME_TYPEDEF_H_
#define MENY_MODULE_DATE_TIME_DATE_TIME_TYPEDEF_H_

#include <stdint.h>

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t week; // день недели 1..7
} Date_TypeDef;

typedef struct {
    uint8_t Seconds;
    uint8_t Minutes;
    uint8_t Hours;
} RTC_TimeTypeDef;

#endif /* MENY_MODULE_DATE_TIME_DATE_TIME_TYPEDEF_H_ */
