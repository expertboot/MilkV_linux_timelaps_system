/*
 * time_calc.h
 *
 *  Created on: 23 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef TIME_CALC_TIME_CALC_H_
#define TIME_CALC_TIME_CALC_H_

#include <stdint.h>
#include "../MENY/module_date_time/module_date_time.h"

typedef enum {
    TIME_LESS = -1,  // первое время меньше второго.
    TIME_EQUAL = 0,  // оба времени равны.
    TIME_GREATER = 1 // первое время больше второго.
} TimeCompareResult;

 // сложить два времени
RTC_TimeTypeDef RTC_AddTimes(RTC_TimeTypeDef time1, RTC_TimeTypeDef time2, uint8_t *flag_newDay);
uint8_t ZellersCongruence(Date_TypeDef sDate); // Функция для вычисления дня недели по алгоритму Зеллера
TimeCompareResult CompareTime(RTC_TimeTypeDef* time1, RTC_TimeTypeDef* time2);
uint8_t IsLeapYear(uint16_t year); // Проверка, является ли год високосным
uint8_t GetDaysInMonth(uint8_t month, uint16_t year); // Получить количество дней в месяце
void IncrementDate(Date_TypeDef *date);// Функция инкремента даты (увеличение на 1 день)


#endif /* TIME_CALC_TIME_CALC_H_ */
