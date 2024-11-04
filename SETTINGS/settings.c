#include "settings.h"


#include <stdio.h>

void print_param(struct param *params) {
    // Вывод времени интервала
    printf("Time Interval: %02d:%02d:%02d\n", 
           params->time_interval.Hours, 
           params->time_interval.Minutes, 
           params->time_interval.Seconds);
    
    // Вывод времени старта
    printf("Time Start: %02d:%02d:%02d\n", 
           params->time_start.Hours, 
           params->time_start.Minutes, 
           params->time_start.Seconds);
    
    // Вывод времени остановки
    printf("Time Stop: %02d:%02d:%02d\n", 
           params->time_stop.Hours, 
           params->time_stop.Minutes, 
           params->time_stop.Seconds);
    
    // Вывод текущего времени
    printf("Current Time: %02d:%02d:%02d\n", 
           params->time_rtc.Hours, 
           params->time_rtc.Minutes, 
           params->time_rtc.Seconds);
    
    // Вывод текущей даты
    printf("Current Date: %04d-%02d-%02d\n", 
           params->date_rtc.year, 
           params->date_rtc.month, 
           params->date_rtc.day);
    
    // Вывод Action Week (в шестнадцатеричном формате)
    printf("Action Week: %02X\n", params->action_week);
}


// Функция записи параметров в текстовый файл
int settings_write(struct param *params) {
    FILE *file = fopen(NAMEFILESETTINGS, "w"); // Открываем файл для записи в текстовом формате
    if (file == NULL) {
        perror("Ошибка открытия файла настроек для записи");
        return -1;
    }

    // Запись структур в текстовом формате
    fprintf(file, "Time Interval: %02d:%02d:%02d\n",
            params->time_interval.Hours,
            params->time_interval.Minutes,
            params->time_interval.Seconds);
    fprintf(file, "Time Start: %02d:%02d:%02d\n",
            params->time_start.Hours,
            params->time_start.Minutes,
            params->time_start.Seconds);
    fprintf(file, "Time Stop: %02d:%02d:%02d\n",
            params->time_stop.Hours,
            params->time_stop.Minutes,
            params->time_stop.Seconds);
    fprintf(file, "Current Time: %02d:%02d:%02d\n",
            params->time_rtc.Hours,
            params->time_rtc.Minutes,
            params->time_rtc.Seconds);
    fprintf(file, "Current Date: %04d-%02d-%02d\n",
            params->date_rtc.year,
            params->date_rtc.month,
            params->date_rtc.day);
    fprintf(file, "Action Week: %02X\n", params->action_week); // Запись в шестнадцатеричном формате

    fclose(file);
	
   printf("Настройки записаны в файл \n");
   print_param(params);

	return 0;
}


// Функция чтения параметров из текстового файла
int settings_read(struct param *params) {
    FILE *file = fopen(NAMEFILESETTINGS, "r"); // Открываем файл для чтения в текстовом формат
											   //
    if (file == NULL) {
        perror("Ошибка открытия файла настроет для чтения");
        return -1;
    }

	// Чтение структур из текстового формата
	fscanf(file, "Time Interval: %hhu:%hhu:%hhu\n",
		   &params->time_interval.Hours,
		   &params->time_interval.Minutes,
		   &params->time_interval.Seconds);

	fscanf(file, "Time Start: %hhu:%hhu:%hhu\n",
		   &params->time_start.Hours,
		   &params->time_start.Minutes,
		   &params->time_start.Seconds);

	fscanf(file, "Time Stop: %hhu:%hhu:%hhu\n",
		   &params->time_stop.Hours,
		   &params->time_stop.Minutes,
		   &params->time_stop.Seconds);

	fscanf(file, "Current Time: %hhu:%hhu:%hhu\n",
		   &params->time_rtc.Hours,
		   &params->time_rtc.Minutes,
		   &params->time_rtc.Seconds);

	fscanf(file, "Current Date: %hu-%hhu-%hhu\n",
		   &params->date_rtc.year,
		   &params->date_rtc.month,
		   &params->date_rtc.day);

	fscanf(file, "Action Week: %02hhX\n", &params->action_week);

	
   printf("Настройки прочитаны из файла \n");
   print_param(params);



	fclose(file);
	return 0;
}



