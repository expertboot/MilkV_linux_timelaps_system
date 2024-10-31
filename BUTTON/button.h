#ifndef __BUTTON_H
#define __BUTTON_H

#include <stdint.h>

#define LONG_PRESS 300 // 500мс

typedef enum{
	BUTTON_NORM ,
	BUTTON_INV
}inv_but_TypeDef;

typedef enum{
	PRESS_NO ,  // не нажата
	PRESS_FAST, // кратковременное нажатие
	PRESS_LONG  // длинное нажатие
}pressButton_TypeDef;

typedef struct{
	int 	pin;
	long    time;
	uint8_t flagPres;
	inv_but_TypeDef  flagInv;
	pressButton_TypeDef status;
} button_TypeDef;


//button_TypeDef button_encoder; // создаем кнопку энкодера
//button_TypeDef button_1;
//button_TypeDef button_2;
//button_TypeDef button_3;

void buttonInit(button_TypeDef* button);
int buttonTimeRead(button_TypeDef* button, int trigerTime); // возвращает время нажатия кнопки, когда ее отпустили, необходимо опрашивать постоянно.
uint8_t buttonRead(button_TypeDef* button); // простой опрос кнопки, нажата или нет
pressButton_TypeDef readButtonStatus(button_TypeDef* button); // возвращает тип нажатие на кнопку
/*
 * ПРИМЕР:
  #include "button.h"

	  button_TypeDef button_1;
	  button_encoder.pin = GPIO_Pin_1;
	  button_encoder.flagInv = BUTTON_INV;
	  buttonInit(&button_1);

	  button_TypeDef button_2;
	  button_encoder.pin = GPIO_Pin_2;
	  button_encoder.flagInv = BUTTON_INV;
	  buttonInit(&button_2);


	unsigned int buton_cnt = buttonTimeRead(&button_1,500);
	if (buton_cnt)
	{
		if (buton_cnt == 500)
		{
		// что то делаем если кнопку удерживали больше 500 мс
		}
		else
		{
		 // что то делаем если кнопку кратковременно нажали
		}

*/


#endif
