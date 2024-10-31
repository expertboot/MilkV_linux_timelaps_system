/*
 * module_text.h
 *
 *  Created on: 26 сент. 2024 г.
 *      Author: ro_rudenko
 */

#ifndef MENY_MODULE_TEXT_MODULE_TEXT_H_
#define MENY_MODULE_TEXT_MODULE_TEXT_H_

#include "../menu_conf.h"
//#include "../meny.h"
#include "../../BUTTON/button.h"

uint8_t add_lineMenu_text(config_meny *config, char* name1, char* name2, uint8_t* value, char **textList, uint8_t size, void* postFunc);
void top_meny_changeListText(const config_meny *conf_meny, uint8_t numline, const FontDef_t * font);

#endif /* MENY_MODULE_TEXT_MODULE_TEXT_H_ */
