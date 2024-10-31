/*
 * module_float.h
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef MENY_MODULE_FLOAT_MODULE_FLOAT_H_
#define MENY_MODULE_FLOAT_MODULE_FLOAT_H_

#include "../menu_conf.h"
#include "../../BUTTON/button.h"
#include <stdint.h>
//#include "meny.h"


uint8_t add_lineMenu_float(config_meny *config, char* name1, char* name2, float* value, float min, float max, void* postFunc);
void top_meny_changeFloat(const config_meny *conf_meny, uint8_t numline);
uint8_t float_convert_changeStrok(const config_meny *conf_meny, uint8_t numStroke, char* ext_buffer);

int   n_tu1(int number, int count);
void  float_to_string(float f, char *r, uint8_t size_after_dot);
float stroke_to_float(char* buffer);

void incriment_ascii(char* buffer, uint8_t pos_number, uint16_t cnt);
void decriment_ascii(char* buffer, uint8_t pos_number, uint16_t cnt);

#endif /* MENY_MODULE_FLOAT_MODULE_FLOAT_H_ */
