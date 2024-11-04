/*
 * time_calc.c
 *
 *  Created on: 23 сент. 2024 г.
 *      Author: ro_rudenko
 */

#include "time_calc.h"

// складывает два времени с учетом перехода на новый день
RTC_TimeTypeDef RTC_AddTimes(RTC_TimeTypeDef time1, RTC_TimeTypeDef time2, uint8_t *flag_newDay)
{
    RTC_TimeTypeDef result;

    // Обнуляем флаг
    *flag_newDay = 0;

    // Сложение секунд
    uint8_t secondsSum = time1.Seconds + time2.Seconds;
    result.Seconds = secondsSum % 60;
    uint8_t carryMinutes = secondsSum / 60;  // Переполнение секунд

    // Сложение минут
    uint8_t minutesSum = time1.Minutes + time2.Minutes + carryMinutes;
    result.Minutes = minutesSum % 60;
    uint8_t carryHours = minutesSum / 60;  // Переполнение минут

    // Сложение часов
    uint8_t hoursSum = time1.Hours + time2.Hours + carryHours;
    result.Hours = hoursSum % 24;  // Переполнение часов

    // Если сумма часов больше или равна 24, произошло переполнение дня
    if (hoursSum >= 24) {
        *flag_newDay = 1;  // Устанавливаем флаг, если произошел переход на новый день
    }

    return result;
}


// Функция для вычисления дня недели по алгоритму Зеллера
uint8_t ZellersCongruence(Date_TypeDef sDate) {

	int day = sDate.day;
	int month = sDate.month;
	int year = sDate.year;
    // Если месяц январь или февраль, рассматриваем его как 13-й или 14-й месяц предыдущего года
    if (month < 3) {
        month += 12;
        year -= 1;
    }

    int K = year % 100; // Последние две цифры года
    int J = year / 100; // Первые две цифры года

    // Применяем формулу Зеллера
    int h = (day + (13 * (month + 1)) / 5 + K + (K / 4) + (J / 4) - 2 * J) % 7;

    // Преобразование результата, чтобы неделя начиналась с понедельника (0 = понедельник, 6 = воскресенье)
    uint8_t dayOfWeek = (h + 5) % 7; // Сдвигаем на 2 позиции вперёд (0 = понедельник)

    //uint8_t dayOfWeek =  (h + 7) % 7; // Преобразование результата для удобства (0 = суббота, 1 = воскресенье, 2 = понедельник и т.д.)

    return dayOfWeek;
}

/*
TIME_LESS: первое время меньше второго.
TIME_EQUAL: оба времени равны.
TIME_GREATER: первое время больше второго.
*/
TimeCompareResult CompareTime(RTC_TimeTypeDef* time1, RTC_TimeTypeDef* time2) {
    // Сравниваем часы
    if (time1->Hours < time2->Hours) {
        return TIME_LESS;
    } else if (time1->Hours > time2->Hours) {
        return TIME_GREATER;
    }

    // Сравниваем минуты
    if (time1->Minutes < time2->Minutes) {
        return TIME_LESS;
    } else if (time1->Minutes > time2->Minutes) {
        return TIME_GREATER;
    }

    // Сравниваем секунды
    if (time1->Seconds < time2->Seconds) {
        return TIME_LESS;
    } else if (time1->Seconds > time2->Seconds) {
        return TIME_GREATER;
    }

    // Если часы, минуты и секунды одинаковы, то возвращаем TIME_EQUAL
    return TIME_EQUAL;
}
  //  int dayOfWeek = ZellersCongruence(day, month, year);
  //  char* daysOfWeek[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};


// Проверка, является ли год високосным
uint8_t IsLeapYear(uint16_t year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1;
    }
    return 0;
}

// Получить количество дней в месяце
uint8_t GetDaysInMonth(uint8_t month, uint16_t year) {
    switch (month) {
        case 1:  // Январь
        case 3:  // Март
        case 5:  // Май
        case 7:  // Июль
        case 8:  // Август
        case 10: // Октябрь
        case 12: // Декабрь
            return 31;
        case 4:  // Апрель
        case 6:  // Июнь
        case 9:  // Сентябрь
        case 11: // Ноябрь
            return 30;
        case 2:  // Февраль
            return IsLeapYear(year) ? 29 : 28;
        default:
            return 30; // Дефолтное значение, на случай ошибки
    }
}

// Функция инкремента даты (увеличение на 1 день)
void IncrementDate(Date_TypeDef *date) {
    // Увеличиваем день на 1
    date->day++;

    // Получаем количество дней в текущем месяце
    uint8_t daysInMonth = GetDaysInMonth(date->month, date->year);

    // Проверяем, не вышли ли за пределы количества дней в месяце
    if (date->day > daysInMonth) {
        date->day = 1;  // Переход на следующий месяц
        date->month++;

        // Если вышли за пределы декабря, переходим на следующий год
        if (date->month > 12) {
            date->month = 1;
            date->year++;
        }
    }

    // Увеличиваем день недели
    date->week++;
    if (date->week > 7) {
        date->week = 1;  // Переход на понедельник
    }
}
