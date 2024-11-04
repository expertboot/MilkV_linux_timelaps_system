/*
 * utility.c
 *
 *  Created on: Sep 9, 2024
 *      Author: ro_rudenko
 */

#include "utility.h"


void debug_string(const char* string)  // Измените на const char*, если строка не будет изменяться
{
    printf("%s\n", string);  
}

void ds3231_write_rtc(RTC_TimeTypeDef sTime)
{
	 DS3231_SetHour(sTime.Hours);
	 DS3231_SetMinute(sTime.Minutes);
	 DS3231_SetSecond(sTime.Seconds);
}

RTC_TimeTypeDef ds3231_read_rtc(void)
{
	RTC_TimeTypeDef sTime;
    sTime.Hours =   DS3231_GetHour();
    sTime.Minutes = DS3231_GetMinute();
    sTime.Seconds = DS3231_GetSecond();
	return sTime;
}

void ds3231_write_date(Date_TypeDef sDate )
{
	DS3231_SetDate(sDate.day);
	DS3231_SetMonth(sDate.month);
	DS3231_SetDayOfWeek(sDate.week);
	DS3231_SetYear(sDate.year);
}

Date_TypeDef ds3231_read_date(void)
{
	Date_TypeDef sDate;
	sDate.week = DS3231_GetDayOfWeek();
	sDate.day = DS3231_GetDate();
	sDate.month = DS3231_GetMonth();
	sDate.year = DS3231_GetYear();
	return sDate;
}



void set_power_camera(uint8_t state){
}


void ds3231_write_alarm(RTC_TimeTypeDef timeAlarm) // date_day 0..31
{
	//RTC_TimeTypeDef set_time = read_rtc();

	//DS3231_EnableAlarm1(DS3231_DISABLED);
	//Set to alarm interrupt mode.

	DS3231_ClearAlarm1Flag(); // сбрасываем флаг срабатывания будильника
	if(DS3231_GetAlarm1Flag() == 1) {// если не сбросился то повтаряем процедуру
		DS3231_ClearAlarm1Flag();
		debug_string("DS3231_ClearAlarm1Flag() ERROR");
//		set_power_camera(1);
	//	HAL_Delay(30);
	}

	DS3231_EnableAlarm1(DS3231_ENABLED);
	DS3231_State state = DS3231_CheckAlarm1Enabled();
	if(state != DS3231_ENABLED){
		DS3231_EnableAlarm1(DS3231_ENABLED);
		debug_string("Alarm1Status != DS3231_ENABLED");
		set_power_camera(1);
//		HAL_Delay(30);
	}


	DS3231_InterruptMode mode = DS3231_GetInterruptMode();
	if(mode != DS3231_ALARM_INTERRUPT){
		DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
		debug_string("mode != DS3231_ALARM_INTERRUPT");
		set_power_camera(1);
//		HAL_Delay(30);
	}


	//Test alarm 1
	DS3231_SetAlarm1Mode(DS3231_A1_MATCH_S_M_H);
	DS3231_Alarm1Mode alarmMode = DS3231_GetAlarm1Mode();
	if(alarmMode != DS3231_A1_MATCH_S_M_H){
		DS3231_SetAlarm1Mode(DS3231_A1_MATCH_S_M_H);
		debug_string("alarmMode != DS3231_A1_MATCH_S_M_H");
		set_power_camera(1);
//		HAL_Delay(30);
	}


	DS3231_EnableAlarm1(DS3231_ENABLED);
	state = DS3231_CheckAlarm1Enabled();
	if(state != DS3231_ENABLED){
		DS3231_EnableAlarm1(DS3231_ENABLED);
		debug_string("Alarm1Status != DS3231_ENABLED");
		set_power_camera(1);
//		HAL_Delay(30);
	}



	DS3231_SetAlarm1Second(timeAlarm.Seconds);
	DS3231_SetAlarm1Minute(timeAlarm.Minutes);
	DS3231_SetAlarm1Hour(timeAlarm.Hours);

	uint8_t second = DS3231_GetAlarm1Second();
	uint8_t minute = DS3231_GetAlarm1Minute();
	uint8_t hours = DS3231_GetAlarm1Hour();

	while(timeAlarm.Seconds != second)
	{
//		set_power_raspberry();
		DS3231_SetAlarm1Second(timeAlarm.Seconds);
		second = DS3231_GetAlarm1Second();
		debug_string("timeAlarm.Seconds != second");
		set_power_camera(1);
//		HAL_Delay(30);
	}

	while(timeAlarm.Minutes != minute)
	{
//		set_power_raspberry(GPIO_PIN_SET);
		DS3231_SetAlarm1Minute(timeAlarm.Minutes);
		minute = DS3231_GetAlarm1Minute();
		debug_string("timeAlarm.Minutes != minute");
		set_power_camera(1);
//		HAL_Delay(30);
	}

	while(timeAlarm.Hours != hours)
	{
	//	set_power_raspberry(GPIO_PIN_SET);
		DS3231_SetAlarm1Hour(timeAlarm.Hours);
		hours = DS3231_GetAlarm1Hour();
		debug_string("timeAlarm.Hours != hours");
		set_power_camera(1);
	//	HAL_Delay(30);
	}

	/*
	DS3231_EnableAlarm1(DS3231_ENABLED);
	DS3231_State state = DS3231_GetAlarm1Status();
	if(state != DS3231_ENABLED){
		DS3231_EnableAlarm1(DS3231_ENABLED);
		debug_string("Alarm1Status != DS3231_ENABLED");
	}
	*/

	state = DS3231_CheckAlarm1Enabled();
	if(state != DS3231_ENABLED){
		DS3231_EnableAlarm1(DS3231_ENABLED);
		debug_string("2 - Alarm1Status != DS3231_ENABLED");
		set_power_camera(1);
	}

	//DS3231_SetAlarm1Date(11);
	//DS3231_SetAlarm1Date(date_day);
	//DS3231_SetAlarm1Day();
}


// возвращает время и дату следующего снимка относительно текущего времени
void calculate_next_time(RTC_TimeTypeDef *time, Date_TypeDef* date)
{
	 struct param settings;
     settings_write(&settings);

	  RTC_TimeTypeDef sTime_curent = ds3231_read_rtc();
	  Date_TypeDef sDate_curent = ds3231_read_date();
	  RTC_TimeTypeDef sTime_start = settings.time_start;
	  RTC_TimeTypeDef sTime_stop = settings.time_stop;
	  RTC_TimeTypeDef sTime_interval = settings.time_interval;
	  uint8_t week_curent = ZellersCongruence(sDate_curent);
	  uint8_t action_week = settings.action_week;
	  uint8_t newDay = 0;
      // вычисляем новое время и дату для следующего снимка
	  RTC_TimeTypeDef newTime = RTC_AddTimes(sTime_curent, sTime_interval, &newDay);

	  if(newDay)
		  sDate_curent = next_action_date(sDate_curent);
	  else // если остались в этом дне
	  {
		  if( (1 << week_curent) & action_week)// этот день есть в работе
		  {
			if(CompareTime(&newTime, &sTime_stop) == TIME_LESS)  // Проверка временного диапазона
				if(CompareTime(&newTime, &sTime_start) == TIME_GREATER)
				{
					// Если новое время находится в допустимом диапазоне
					time->Hours = newTime.Hours;
					time->Minutes = newTime.Minutes;
					time->Seconds = newTime.Seconds;
					date->day = sDate_curent.day;
					date->month = sDate_curent.month;
					date->year = sDate_curent.year;
					return; // ок
				}
		  }
		  // переходим на следующий день работы
		  sDate_curent = next_action_date(sDate_curent); // если этого дня недели нету в работе
	  }

	  date->day = sDate_curent.day;
	  date->month = sDate_curent.month;
	  date->year = sDate_curent.year;
	  time->Hours = sTime_start.Hours;
	  time->Minutes = sTime_start.Minutes;
	  time->Seconds = sTime_start.Seconds;
	  return;
}

// возврашает следующий рабочий день
Date_TypeDef next_action_date(Date_TypeDef sDate_curent)
{
	  struct param settings;
      settings_read(&settings);

	  IncrementDate(&sDate_curent); // переходим на новый день
	  uint8_t week_curent = ZellersCongruence(sDate_curent);
	  uint8_t action_week = settings.action_week;

	  if( (1 << week_curent) & action_week)// этот день есть в работе
	  {
		  return sDate_curent;

	  } else //нету, узнаем какой следующей день недели активен
	  {
		  for(int i = 0; i<7; i++)// ограничиваем цикл в одну неделю
		  {
			  IncrementDate(&sDate_curent);
			  week_curent = ZellersCongruence(sDate_curent);
			  // проверка если он активен
			  if((1 << week_curent) & action_week)  break; // нашли рабочий день
		  } // ТУТ НАДО ОБРАБОТАЬ НЕНАХОД дня !!!!!!!!!!!!
	  }

	  return sDate_curent;
}


