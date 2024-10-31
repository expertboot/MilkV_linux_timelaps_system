/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for STM32f10x: Alexander Lutsai<s.lyra@ya.ru>

   ----------------------------------------------------------------------
   	Copyright (C) Alexander Lutsai, 2016
    Copyright (C) Tilen Majerle, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
 */

/*
	Что бы CubeIDE смогла работать с кириллицей в привычном для нас режиме (1 символ — 1 байт), необходимо указать это в настройках среды программирования.
	Нажимаем: Project->Properties. В появившемся окне выбираем С/С++ Build->Settings. Затем переходим на вкладку Tool Settings, находим MCU GCC Compiler -> Miscellaneous.
	Здесь добавляем строчку : -finput-charset=UTF-8 -fexec-charset=cp1251
	Нажимаем кнопку Aplay and Close.
 */
#ifndef SCREEN_GRAPHICS_H
#define SCREEN_GRAPHICS_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <unistd.h>
#include "fonts.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>
//#include "button.h"  // драйвер кнопки

// тип используемого драйвера
//#define DISPLAY_SH1106_SPI
#define DISPLAY_SSD1306



#ifdef DISPLAY_SH1106_SPI
#include "sh1106_spi.h"
#endif

#ifdef DISPLAY_SSD1306
#include "../OLED_I2C/ssd1306_i2c.h"
#endif

#define GRAPHICS_WIDTH   128 // ширина экрана в пикселях
#define GRAPHICS_HEIGHT  64  // высота экрана в пикселях

//static uint8_t GRAPHICS_Buffer[GRAPHICS_WIDTH  * GRAPHICS_HEIGHT / 8];
extern uint8_t GRAPHICS_Buffer[];


typedef enum {
	GRAPHICS_COLOR_BLACK =  0x00,
	GRAPHICS_COLOR_WHITE =  0x01,
	GRAPHICS_COLOR_INVERT = 0x02
} GRAPHICS_COLOR_t;

typedef enum {
	GRAPHICS_REDY =  0x00,
	GRAPHICS_BUSY =  0x01,
} GRAPHICS_BUSY_UPDATE_t;

/**
 * @brief  Initializes SSD1306 LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - > 0: LCD initialized OK and ready to use
 */
uint8_t GRAPHICS_Init(void);// возвращет 1 если ок

/**
 * @brief  Updates buffer from internal RAM to LCD
 * @note   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
 * @param  None
 * @retval None
 */
void GRAPHICS_UpdateScreen(void);

GRAPHICS_BUSY_UPDATE_t GRAPHICS_isBusy(void);

/**
 * @brief  Toggles pixels invertion inside internal RAM
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  None
 * @retval None
 */
void GRAPHICS_ToggleInvert(void);

/**
 * @brief  Fills entire LCD with desired color
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  Color: Color to be used for screen fill. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_Fill(GRAPHICS_COLOR_t Color);

/**
 * @brief  Draws pixel at desired location
 * @note   @ref SSD1306_UpdateScreen() must called after that in order to see updated LCD screen
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @param  color: Color to be used for screen fill. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_DrawPixel(uint16_t x, uint16_t y, GRAPHICS_COLOR_t color);

/**
 * @brief  Sets cursor pointer to desired location for strings
 * @param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
 * @retval None
 */
void GRAPHICS_GotoXY(uint16_t x, uint16_t y);

uint8_t GRAPHICS_getY(void);
uint8_t GRAPHICS_getX(void);
/**
 * @brief  Puts character to internal RAM
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  ch: Character to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval Character written
 */
char GRAPHICS_Putc(char ch, FontDef_t* Font, GRAPHICS_COLOR_t color);

/**
 * @brief  Puts string to internal RAM
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  *str: String to be written
 * @param  *Font: Pointer to @ref FontDef_t structure with used font
 * @param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval Zero on success or character value when function failed
 */
char GRAPHICS_Puts(char* str, FontDef_t* Font, GRAPHICS_COLOR_t color);


uint8_t GRAPHICS_lengtchPixChar(char ch, FontDef_t* Font);



/**
 * @brief  Draws line on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, GRAPHICS_COLOR_t c);

/**
 * @brief  Draws rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, GRAPHICS_COLOR_t c);

/**
 * @brief  Draws filled rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, GRAPHICS_COLOR_t c);

/**
 * @brief  Draws triangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, GRAPHICS_COLOR_t color);

/**
 * @brief  Draws circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_DrawCircle(int16_t x0, int16_t y0, int16_t r, GRAPHICS_COLOR_t c);

/**
 * @brief  Draws filled circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void GRAPHICS_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, GRAPHICS_COLOR_t c);

// возвращает длину строки в пикселях
uint8_t GRAPHICS_lengtchPixStroke(char *strok, FontDef_t* Font);

// печать строки по центру экрана
// y - кордината строки по вертикали
void GRAPHICS_centerLine_Puts(uint8_t y_pix, char * stroke, FontDef_t *font , GRAPHICS_COLOR_t color);

// вывод строки с ограничением по длинне пикселей, остальная часть строки не выводится
char GRAPHICS_PutsToLimit(char* str, int8_t x_limitPix, FontDef_t* Font, GRAPHICS_COLOR_t color);
/* C++ detection */
#ifdef __cplusplus
};
#endif

#endif
