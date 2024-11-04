#ifndef  _SETTINGS_H_
#define  _SETTINGS_H_

#include <stdio.h>
#include <stdint.h>
#include "../MENY/module_date_time/date_time_typedef.h"
#include "../MENY/parametr.h"

#define NAMEFILESETTINGS "timlips_settings.txt"

int settings_write(struct param *params);
int settings_read(struct param *params);

#endif 

