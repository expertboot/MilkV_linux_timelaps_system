#ifndef _ENCODER_H_
#define _ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "wiringx.h"
#include <pthread.h>

int encoderInit(int gpio_a, int gpio_b);
int encoderRead(void);
void encoderWrite(int value);
void encoderClose(void);

#ifdef __cplusplus
}
#endif

#endif

