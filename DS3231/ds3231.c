#include "ds3231.h"
#include <linux/i2c.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h> // системный вызов функции i2c
#include <wiringx.h>
//#include <stdint.h>
#include <stdio.h>



#ifdef __cplusplus
extern "C"{
#endif



//#define I2C_DEV "/dev/i2c-1"

char str_dev_i2c[12];
int fd_ds3231_i2c;


int DS3231_Init( char* dev_i2c) {

//	fd_ds3231_i2c = fd_i2c;
    // Используем strncpy для копирования строки
    strncpy(str_dev_i2c, dev_i2c, sizeof(str_dev_i2c) - 1);
    // Убедимся, что строка завершена нулевым символом
    str_dev_i2c[sizeof(str_dev_i2c) - 1] = '\0';

	if(wiringXSetup("milkv_duo", NULL) == -1) {
         wiringXGC();
         return -1;
     }

     if ((fd_ds3231_i2c = wiringXI2CSetup(str_dev_i2c, DS3231_I2C_ADDR)) <0) {
         printf("I2C Setup failed: %i\n", fd_ds3231_i2c);
         return -1;
     }

	DS3231_EnableAlarm1(DS3231_DISABLED);
	DS3231_EnableAlarm2(DS3231_DISABLED);
	DS3231_ClearAlarm1Flag();
	DS3231_ClearAlarm2Flag();
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);

	return 0;
}

/**
 * @brief Set the byte in the designated DS3231 register to value.
 * @param regAddr Register address to write.
 * @param val Value to set, 0 to 255.
 */
void DS3231_SetRegByte(uint8_t regAddr, uint8_t val) {
	
     union i2c_smbus_data data;
     data.byte = val;
	 struct i2c_smbus_ioctl_data args;
     args.read_write = I2C_SMBUS_WRITE;
     args.command = regAddr;
     args.size = I2C_SMBUS_BYTE_DATA;
     args.data = &data;
     ioctl(fd_ds3231_i2c, I2C_SMBUS, &args);
}

/**
 * @brief Gets the byte in the designated DS3231 register.
 * @param regAddr Register address to read.
 * @return Value stored in the register, 0 to 255.
 */
uint8_t DS3231_GetRegByte(uint8_t regAddr) {
/*	uint8_t val;
	HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &regAddr, 1, DS3231_TIMEOUT);
	HAL_I2C_Master_Receive(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &val, 1, DS3231_TIMEOUT);
	return val;

*/
  /*  union i2c_smbus_data data;
    if(i2c_smbus_access(fd, I2C_SMBUS_READ, regAddr, I2C_SMBUS_WORD_DATA, &data) < 0) {
        return -1;
    } else {
        return 0x0FFFF & data.word;
    }
*/
	  union i2c_smbus_data data;
      struct i2c_smbus_ioctl_data args;
      __s32 err;
      args.read_write = I2C_SMBUS_READ;
      args.command = regAddr;
      args.size = I2C_SMBUS_BYTE_DATA;
      args.data = &data;
      err = ioctl(fd_ds3231_i2c, I2C_SMBUS, &args);
     // if (err == -1) err = -errno;
      return data.byte;
}

uint8_t DS3231_GetRegByte_status_i2c(uint8_t regAddr, int *error) {

/*
	uint8_t val;
	*status_i2c = HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &regAddr, 1, DS3231_TIMEOUT);
	if(*status_i2c != HAL_OK) return 0;
	*status_i2c = HAL_I2C_Master_Receive(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &val, 1, DS3231_TIMEOUT);
	return val;
*/
	  union i2c_smbus_data data;
      struct i2c_smbus_ioctl_data args;
      __s32 err;
      args.read_write = I2C_SMBUS_READ;
      args.command = regAddr;
      args.size = I2C_SMBUS_BYTE_DATA;
      args.data = &data;
      *error = ioctl(fd_ds3231_i2c, I2C_SMBUS, &args);
     // if (err == -1) err = -errno;
      return data.byte;
}

/**
 * @brief Enables battery-backed square wave output at the INT#/SQW pin.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableBatterySquareWave(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xbf) | ((enable & 0x01) << DS3231_BBSQW));
}

/**
 * @brief Set the interrupt mode to either alarm interrupt or square wave interrupt.
 * @param mode Interrupt mode to set, DS3231_ALARM_INTERRUPT or DS3231_SQUARE_WAVE_INTERRUPT.
 */
void DS3231_SetInterruptMode(DS3231_InterruptMode mode){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfb) | ((mode & 0x01) << DS3231_INTCN));
}

DS3231_InterruptMode DS3231_GetInterruptMode(void) {
    // Получаем текущее значение регистра управления
    uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
    // Извлекаем биты, отвечающие за режим прерывания
    uint8_t mode_bit = (control >> DS3231_INTCN) & 0x01;
    // Преобразуем бит в перечисление
    return (DS3231_InterruptMode)mode_bit;
}

/**
 * @brief Set frequency of the square wave output
 * @param rate Frequency to set, DS3231_1HZ, DS3231_1024HZ, DS3231_4096HZ or DS3231_8192HZ.
 */
void DS3231_SetRateSelect(DS3231_Rate rate){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xe7) | ((rate & 0x03) << DS3231_RS1));
}

/**
 * @brief Enables clock oscillator.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableOscillator(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0x7f) | (((!enable) & 0x01) << DS3231_EOSC));
}

/**
 * @brief Enables alarm 2.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableAlarm2(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfd) | ((enable & 0x01) << DS3231_A2IE));
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}

/**
 * @brief Clears alarm 2 matched flag. Matched flags must be cleared before the next match or the next interrupt will be masked.
 */
void DS3231_ClearAlarm2Flag(){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfd;
	DS3231_SetRegByte(DS3231_REG_STATUS, status & ~(0x01 << DS3231_A2F));
}

/**
 * @brief Set alarm 2 minute to match. Does not change alarm 2 matching mode.
 * @param minute Minute, 0 to 59.
 */
void DS3231_SetAlarm2Minute(uint8_t minute){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_MINUTE) & 0x80;
	uint8_t a2m2 = temp | (DS3231_EncodeBCD(minute) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_MINUTE, a2m2);
}

/**
 * @brief Set alarm 2 hour to match. Does not change alarm 2 matching mode.
 * @param hour Hour to match in 24h format, 0 to 23.
 */
void DS3231_SetAlarm2Hour(uint8_t hour_24mode){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_HOUR) & 0x80;
	uint8_t a2m3 = temp | (DS3231_EncodeBCD(hour_24mode) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_HOUR, a2m3);
}

/**
 * @brief Set alarm 2 date. Alarm 2 can only be set to match either date or day. Does not change alarm 2 matching mode.
 * @param date Date, 0 to 31.
 */
void DS3231_SetAlarm2Date(uint8_t date){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_DATE) & 0x80;
	uint8_t a2m4 = temp | (DS3231_EncodeBCD(date) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_DATE, a2m4);
}

/**
 * @brief Set alarm 2 day. Alarm 2 can only be set to match either date or day. Does not change alarm 2 matching mode.
 * @param day Days since last Sunday, 1 to 7.
 */
void DS3231_SetAlarm2Day(uint8_t day){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_DATE) & 0x80;
	uint8_t a2m4 = temp | (0x01 << DS3231_DYDT) | (DS3231_EncodeBCD(day) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_DATE, a2m4);
}

/**
 * @brief Set alarm 2 mode.
 * @param alarmMode Alarm 2 mode, DS3231_A2_EVERY_M, DS3231_A2_MATCH_M, DS3231_A2_MATCH_M_H, DS3231_A2_MATCH_M_H_DATE or DS3231_A2_MATCH_M_H_DAY.
 */
void DS3231_SetAlarm2Mode(DS3231_Alarm2Mode alarmMode){
	uint8_t temp;
	temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_MINUTE, temp | (((alarmMode >> 0) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_HOUR, temp | (((alarmMode >> 1) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_DATE, temp | (((alarmMode >> 2) & 0x01) << DS3231_AXMY) | (alarmMode & 0x80));
}

/**
 * @brief Enables alarm 1.
 * @param enable Enable, DS3231_ENABLED or DS3231_DISABLED.
 */
void DS3231_EnableAlarm1(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfe) | ((enable & 0x01) << DS3231_A1IE));
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}

DS3231_State DS3231_CheckAlarm1Enabled(void) {
    // Чтение значения из регистра контроля (Control Register)
    uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
    // Проверка статуса флага A1IE (прерывание для будильника 1 включено или нет)
    if (control & (1 << DS3231_A1IE)) {
        return DS3231_ENABLED;  // Прерывание для будильника 1 включено
    } else {
        return DS3231_DISABLED; // Прерывание для будильника 1 выключено
    }
}

/**
 * @brief Clears alarm 1 matched flag. Matched flags must be cleared before the next match or the next interrupt will be masked.
 */
void DS3231_ClearAlarm1Flag(){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfe;
	DS3231_SetRegByte(DS3231_REG_STATUS, status & ~(0x01 << DS3231_A1F));
}

uint8_t DS3231_GetAlarm1Flag(void) {
    // Получаем текущее значение регистра статуса
    uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS);
    // Извлекаем бит, отвечающий за флаг будильника 1 (A1F)
    uint8_t alarm1_flag = (status >> DS3231_A1F) & 0x01;
    // Возвращаем флаг (0, если сброшен; 1, если установлен)
    return alarm1_flag;
}

/**
 * @brief Set alarm 1 second to match. Does not change alarm 1 matching mode.
 * @param second Second, 0 to 59.
 */
void DS3231_SetAlarm1Second(uint8_t second){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_SECOND) & 0x80;
	uint8_t a1m1 = temp | (DS3231_EncodeBCD(second) & 0x7f);
	DS3231_SetRegByte(DS3231_A1_SECOND, a1m1);
}

uint8_t DS3231_GetAlarm1Second(void) {
    // Чтение значения из регистра секунд для будильника 1
    uint8_t a1_second = DS3231_GetRegByte(DS3231_A1_SECOND);
    // Маскируем биты, относящиеся к значению секунд (6 младших бит)
    uint8_t second = DS3231_DecodeBCD(a1_second & 0x7F); // 0x7F маскирует старший бит (A1M1)
    return second;
}

/**
 * @brief Set alarm 1 minute to match. Does not change alarm 1 matching mode.
 * @param minute Minute, 0 to 59.
 */
/*void DS3231_SetAlarm1Minute(uint8_t minute){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x80;
	uint8_t a1m2 = temp | (DS3231_EncodeBCD(minute) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_MINUTE, a1m2);
}*/
void DS3231_SetAlarm1Minute(uint8_t minute) {
    // Чтение текущего значения из регистра минут будильника 1, сохраняем бит A1M2
    uint8_t temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x80;
    // Установка минут с сохранением A1M2, минуты кодируются в 7 битах
    uint8_t a1m2 = temp | (DS3231_EncodeBCD(minute) & 0x7F);
    // Записываем новое значение минут в регистр DS3231_A1_MINUTE
    DS3231_SetRegByte(DS3231_A1_MINUTE, a1m2);
}


uint8_t DS3231_GetAlarm1Minute(void) {
    // Чтение значения из регистра секунд для будильника 1
    uint8_t a1_second = DS3231_GetRegByte(DS3231_A1_MINUTE);
    // Маскируем биты, относящиеся к значению секунд (6 младших бит)
    uint8_t second = DS3231_DecodeBCD(a1_second & 0x7F); // 0x7F маскирует старший бит (A1M1)
    return second;
}

/**
 * @brief Set alarm 1 hour to match. Does not change alarm 1 matching mode.
 * @param hour Hour, 0 to 59.
 */
/*
void DS3231_SetAlarm1Hour(uint8_t hour_24mode){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x80;
	uint8_t a1m3 = temp | (DS3231_EncodeBCD(hour_24mode) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_HOUR, a1m3);
}*/

void DS3231_SetAlarm1Hour(uint8_t hour_24mode) {
    // Чтение текущего значения регистра часа для будильника 1, сохраняем бит A1M3 и формат времени (24/12ч)
    uint8_t temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0xC0;  // Сохраняем старшие биты: A1M3 и формат 24/12 часов
    // Установка часа в 24-часовом формате, применяем маску 0x3F для корректного кодирования в 6 битах
    uint8_t a1m3 = temp | (DS3231_EncodeBCD(hour_24mode) & 0x3F);
    // Записываем новое значение в регистр DS3231_A1_HOUR
    DS3231_SetRegByte(DS3231_A1_HOUR, a1m3);
}

uint8_t DS3231_GetAlarm1Hour(void) {
    // Чтение значения из регистра часов будильника 1
    uint8_t a1m3 = DS3231_GetRegByte(DS3231_A1_HOUR);
    // Извлечение битов, которые кодируют часы, маска 0x3F извлекает младшие 6 бит
    return DS3231_DecodeBCD(a1m3 & 0x3F);
}

/**
 * @brief Set alarm 1 date. Alarm 1 can only be set to match either date or day. Does not change alarm 1 matching mode.
 * @param date Date, 0 to 31.
 */
void DS3231_SetAlarm1Date(uint8_t date){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x80;
	uint8_t a1m4 = temp | (DS3231_EncodeBCD(date) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_DATE, a1m4);
}

/**
 * @brief Set alarm 1 day. Alarm 1 can only be set to match either date or day. Does not change alarm 1 matching mode.
 * @param day Days since last Sunday, 1 to 7.
 */
void DS3231_SetAlarm1Day(uint8_t day){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x80;
	uint8_t a1m4 = temp | (0x01 << DS3231_DYDT) | (DS3231_EncodeBCD(day) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_DATE, a1m4);
}

/**
 * @brief Set alarm 1 mode.
 * @param alarmMode Alarm 1 mode, DS3231_A1_EVERY_S, DS3231_A1_MATCH_S, DS3231_A1_MATCH_S_M, DS3231_A1_MATCH_S_M_H, DS3231_A1_MATCH_S_M_H_DATE or DS3231_A1_MATCH_S_M_H_DAY.
 */
void DS3231_SetAlarm1Mode(DS3231_Alarm1Mode alarmMode) {
    uint8_t temp;
    temp = DS3231_GetRegByte(DS3231_A1_SECOND) & 0x7f;
    DS3231_SetRegByte(DS3231_A1_SECOND, temp | (((alarmMode >> 0) & 0x01) << DS3231_AXMY));

    temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x7f;
    DS3231_SetRegByte(DS3231_A1_MINUTE, temp | (((alarmMode >> 1) & 0x01) << DS3231_AXMY));

    temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x7f;
    DS3231_SetRegByte(DS3231_A1_HOUR, temp | (((alarmMode >> 2) & 0x01) << DS3231_AXMY));

    temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x7f;

    // Добавляем старший бит только если он есть в режиме
    DS3231_SetRegByte(DS3231_A1_DATE, temp | (((alarmMode >> 3) & 0x01) << DS3231_AXMY) | (alarmMode & 0x80));
}


DS3231_Alarm1Mode DS3231_GetAlarm1Mode(void) {
    uint8_t second_reg = DS3231_GetRegByte(DS3231_A1_SECOND);
    uint8_t minute_reg = DS3231_GetRegByte(DS3231_A1_MINUTE);
    uint8_t hour_reg = DS3231_GetRegByte(DS3231_A1_HOUR);
    uint8_t date_reg = DS3231_GetRegByte(DS3231_A1_DATE);

    // Извлекаем биты режима
    uint8_t mode_bits = ((second_reg >> DS3231_AXMY) & 0x01) |
                        (((minute_reg >> DS3231_AXMY) & 0x01) << 1) |
                        (((hour_reg >> DS3231_AXMY) & 0x01) << 2) |
                        (((date_reg >> DS3231_AXMY) & 0x01) << 3);

    // Учитываем старший бит только для режимов, где он используется
    if (mode_bits == DS3231_A1_MATCH_S_M_H_DAY || mode_bits == DS3231_A1_MATCH_S_M_H_DATE) {
        mode_bits |= (date_reg & 0x80); // Старший бит
    }

    // Возвращаем режим будильника
    return (DS3231_Alarm1Mode)mode_bits;
}

/**
 * @brief Check whether the clock oscillator is stopped.
 * @return Oscillator stopped flag (OSF) bit, 0 or 1.
 */
uint8_t DS3231_IsOscillatorStopped(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_OSF) & 0x01;
}

/**
 * @brief Check whether the 32kHz output is enabled.
 * @return EN32kHz flag bit, 0 or 1.
 */
uint8_t DS3231_Is32kHzEnabled(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_EN32KHZ) & 0x01;
}

/**
 * @brief Check if alarm 1 is triggered.
 * @return A1F flag bit, 0 or 1.
 */
uint8_t DS3231_IsAlarm1Triggered(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_A1F) & 0x01;
}

/**
 * @brief Check if alarm 2 is triggered.
 * @return A2F flag bit, 0 or 1.
 */
uint8_t DS3231_IsAlarm2Triggered(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_A2F) & 0x01;
}

/**
 * @brief Gets the current day of week.
 * @return Days from last Sunday, 0 to 6.
 */
uint8_t DS3231_GetDayOfWeek(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DOW));
}

/**
 * @brief Gets the current day of month.
 * @return Day of month, 1 to 31.
 */
uint8_t DS3231_GetDate(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DATE));
}

/**
 * @brief Gets the current month.
 * @return Month, 1 to 12.
 */
uint8_t DS3231_GetMonth(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f);
}

/**
 * @brief Gets the current year.
 * @return Year, 2000 to 2199.
 */
uint16_t DS3231_GetYear(void) {
	uint8_t decYear = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_YEAR));
	uint16_t century = (DS3231_GetRegByte(DS3231_REG_MONTH) >> DS3231_CENTURY) * 100 + 2000;
	return century + decYear;
}

/**
 * @brief Gets the current hour in 24h format.
 * @return Hour in 24h format, 0 to 23.
 */
uint8_t DS3231_GetHour(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_HOUR));
}

/**
 * @brief Gets the current minute.
 * @return Minute, 0 to 59.
 */
uint8_t DS3231_GetMinute(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MINUTE));
}

/**
 * @brief Gets the current second. Clock halt bit not included.
 * @return Second, 0 to 59.
 */
uint8_t DS3231_GetSecond(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_SECOND));
}

/**
 * @brief Set the current day of week.
 * @param dayOfWeek Days since last Sunday, 1 to 7.
 */
void DS3231_SetDayOfWeek(uint8_t dayOfWeek) {
	DS3231_SetRegByte(DS3231_REG_DOW, DS3231_EncodeBCD(dayOfWeek));
}

/**
 * @brief Set the current day of month.
 * @param date Day of month, 1 to 31.
 */
void DS3231_SetDate(uint8_t date) {
	DS3231_SetRegByte(DS3231_REG_DATE, DS3231_EncodeBCD(date));
}

/**
 * @brief Set the current month.
 * @param month Month, 1 to 12.
 */
void DS3231_SetMonth(uint8_t month) {
	uint8_t century = DS3231_GetRegByte(DS3231_REG_MONTH) & 0x80;
	DS3231_SetRegByte(DS3231_REG_MONTH, DS3231_EncodeBCD(month) | century);
}

/**
 * @brief Set the current year.
 * @param year Year, 2000 to 2199.
 */
void DS3231_SetYear(uint16_t year) {
	uint8_t century = (year / 100) % 20;
	uint8_t monthReg = (DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f) | (century << DS3231_CENTURY);
	DS3231_SetRegByte(DS3231_REG_MONTH, monthReg);
	DS3231_SetRegByte(DS3231_REG_YEAR, DS3231_EncodeBCD(year % 100));
}

/**
 * @brief Set the current hour, in 24h format.
 * @param hour_24mode Hour in 24h format, 0 to 23.
 */
void DS3231_SetHour(uint8_t hour_24mode) {
	DS3231_SetRegByte(DS3231_REG_HOUR, DS3231_EncodeBCD(hour_24mode & 0x3f));
}

/**
 * @brief Set the current minute.
 * @param minute Minute, 0 to 59.
 */
void DS3231_SetMinute(uint8_t minute) {
	DS3231_SetRegByte(DS3231_REG_MINUTE, DS3231_EncodeBCD(minute));
}

/**
 * @brief Set the current second.
 * @param second Second, 0 to 59.
 */
void DS3231_SetSecond(uint8_t second) {
	DS3231_SetRegByte(DS3231_REG_SECOND, DS3231_EncodeBCD(second));
}

/**
 * @brief Set the current time.
 * @param hour_24mode Hour in 24h format, 0 to 23.
 * @param minute  Minute, 0 to 59.
 * @param second Second, 0 to 59.
 */
void DS3231_SetFullTime(uint8_t  hour_24mode, uint8_t minute, uint8_t second){
	DS3231_SetHour(hour_24mode);
	DS3231_SetMinute(minute);
	DS3231_SetSecond(second);
}

/**
 * @brief Set the current date, month, day of week and year.
 * @param date Date, 0 to 31.
 * @param month Month, 1 to 12.
 * @param dow Days since last Sunday, 1 to 7.
 * @param year Year, 2000 to 2199.
 */
void DS3231_SetFullDate(uint8_t date, uint8_t month, uint8_t dow, uint16_t year){
	DS3231_SetDate(date);
	DS3231_SetMonth(month);
	DS3231_SetDayOfWeek(dow);
	DS3231_SetYear(year);
}

/**
 * @brief Decodes the raw binary value stored in registers to decimal format.
 * @param bin Binary-coded decimal value retrieved from register, 0 to 255.
 * @return Decoded decimal value.
 */
uint8_t DS3231_DecodeBCD(uint8_t bin) {
	return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

/**
 * @brief Encodes a decimal number to binaty-coded decimal for storage in registers.
 * @param dec Decimal number to encode.
 * @return Encoded binary-coded decimal value.
 */
uint8_t DS3231_EncodeBCD(uint8_t dec) {
	return (dec % 10 + ((dec / 10) << 4));
}

/**
 * @brief Enable the 32kHz output.
 * @param enable Enable, DS3231_ENABLE or DS3231_DISABLE.
 */
void DS3231_Enable32kHzOutput(DS3231_State enable){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfb;
	DS3231_SetRegByte(DS3231_REG_STATUS, status | (enable << DS3231_EN32KHZ));
}

/**
 * @brief Get the integer part of the temperature.
 * @return Integer part of the temperature, -127 to 127.
 */
int8_t DS3231_GetTemperatureInteger(){
	return DS3231_GetRegByte(DS3231_TEMP_MSB);
}

/**
 * @brief Get the fractional part of the temperature to 2 decimal places.
 * @return Fractional part of the temperature, 0, 25, 50 or 75.
 */
uint8_t DS3231_GetTemperatureFraction(){
	return (DS3231_GetRegByte(DS3231_TEMP_LSB) >> 6) * 25;
}

#ifdef __cplusplus
}
#endif
