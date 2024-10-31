#include "button.h"
#include <stdint.h>
//#include <stdio.h>
#include <unistd.h>
//#include <string.h>
#include "wiringx.h"
#include <stdbool.h>
#include <time.h>

void buttonInit(button_TypeDef* button)
{
	pinMode(button->pin, PINMODE_INPUT);
	button->flagPres = 0;
	button->time = 0;
}

// Функция для получения времени в миллисекундах
long get_time_in_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/*
 * пока кнопку нажали и удерживаем, функция возвращает 0.
 * если удерживаем кнопку с выше trigerTime, то функция возвращает время trigerTime.
 * если отпустили кнопку до времени trigerTime, то функция возвращает время нажатия кнопки.
 */
int buttonTimeRead(button_TypeDef* button, int trigerTime)
{
	int time_button = 0;
	int action = digitalRead(button->pin);
	if(button->flagInv == BUTTON_INV) action = action ^ 1;

	if(button->flagPres == 0)
	{
		if(action)
		{
			button->time = get_time_in_ms(); 
			button->flagPres = 1;
			return 0;
		} // только-что нажали кнопку
	};

	if(button->flagPres == 2)
		if(!action){button->flagPres = 0;} // когда удерживаем кнопку с выше trigerTime, а потом отпустили

	if(button->flagPres == 1){
		if(action)
		{
			if((get_time_in_ms() - button->time) > trigerTime)
			{
				time_button = trigerTime;
				button->flagPres = 2;
			}
		}
		else{
			button->flagPres = 0;	
			time_button = get_time_in_ms() - button->time;
		}} // если отпустили кнопку до времени trigerTime

   return time_button;
}

/*
 * простой опрос кнопки, нажата или нет
 */
uint8_t buttonRead(button_TypeDef* button)
{
	uint8_t action = digitalRead(button->pin);
	if(button->flagInv == BUTTON_INV) action = action ^ 1;
	return action;
}

pressButton_TypeDef readButtonStatus(button_TypeDef* button)
{
	int time = buttonTimeRead(button, LONG_PRESS);
	if(time == 0){
	   	return PRESS_NO;
	}
	else
	{
		if(time < LONG_PRESS){
		   	return PRESS_FAST;
		}
		else{
			if(time == LONG_PRESS) return PRESS_LONG;
		}
	}
	return PRESS_NO;
}










