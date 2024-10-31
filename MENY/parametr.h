
#ifndef PARAMETR_H
#define PARAMETR_H

#include "module_date_time/date_time_typedef.h"

// основные параметры для регулировки через меню
struct param
{
	RTC_TimeTypeDef time_interval; // период кадров таймлапса
	RTC_TimeTypeDef time_start;    // время старта таймлапса
	RTC_TimeTypeDef time_stop;     // время старта таймлапса
	RTC_TimeTypeDef time_rtc;      // текущее время
	Date_TypeDef    date_rtc;      // текущая дата
	uint8_t         action_week; // дни недели в который будет работать система. тут биты 0-6 это Пн - Вс
};


#endif

