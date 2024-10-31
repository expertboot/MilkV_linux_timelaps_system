/*
 * param_int.h
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef MODULE_INT_H
#define MODULE_INT_H

#include "../menu_conf.h"
#include "../../BUTTON/button.h"
#include <stdlib.h> // Для malloc

uint8_t add_lineMenu_int(config_meny *config, char* name1, char* name2, int16_t* value, int16_t min, int16_t max, void* postFunc);
void    top_meny_changeInt(const config_meny *conf_meny, uint8_t numline);
uint8_t int_convert_changeStrok(const config_meny *conf_meny, uint8_t numline, char* ext_buffer);

#endif /* MENY_PARAM_INT_PARAM_INT_H_ */
