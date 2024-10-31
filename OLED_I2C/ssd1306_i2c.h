
#ifndef _SSD1306_I2C_H_
#define _SSD1306_I2C_H_

#include <wiringx.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//#define OLED_SH1106             // включает тип контроллера SH1106
#define I2C_DEV "/dev/i2c-1"
#define SSD1306_ADDRESS  0x3C   // I2C address display
#define SSD1306_WIDTH    128    // ширина экрана в пикселях
#define SSD1306_HEIGHT   64     // высота экрана в пикселях
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8) // 1024

extern uint8_t ssd1306_BufferTrnsit[]; // Независымый буфер для транзикации


// I2c address
#define SSD1306_I2C_ADDR       SSD1306_ADDRESS << 1 // 0x3C << 1 = 0x78

// Display commands
#define CHARGEPUMP            0x8D
#define COLUMNADDR            0x21
#define COMSCANDEC            0xC8
#define COMSCANINC            0xC0
#define DISPLAYALLON          0xA5
#define DISPLAYALLON_RESUME   0xA4
#define DISPLAYOFF            0xAE
#define DISPLAYON             0xAF
#define EXTERNALVCC           0x01
#define INVERTDISPLAY         0xA7
#define MEMORYMODE            0x20
#define NORMALDISPLAY         0xA6
#define PAGEADDR              0x22
#define SEGREMAP              0xA0
#define SETCOMPINS            0xDA
#define SETCONTRAST           0x81
#define SETDISPLAYCLOCKDIV    0xD5
#define SETDISPLAYOFFSET      0xD3
#define SETHIGHCOLUMN         0x10
#define SETLOWCOLUMN          0x00
#define SETMULTIPLEX          0xA8
#define SETPRECHARGE          0xD9
#define SETSEGMENTREMAP       0xA1
#define SETSTARTLINE          0x40
#define SETVCOMDETECT         0xDB
#define SWITCHCAPVCC          0x02

#define SSD1306_DisplayOn()             SSD1306_WriteCommand(DISPLAYON)
#define SSD1306_DisplayOff()            SSD1306_WriteCommand(DISPLAYOFF)
#define SSD1306_InvertDisplay()         SSD1306_WriteCommand(INVERTDISPLAY)
#define SSD1306_NormalDisplay()         SSD1306_WriteCommand(NORMALDISPLAY)
#define SSD1306_ResetOrientation()      { SSD1306_WriteCommand(SEGREMAP); SSD1306_WriteCommand(COMSCANINC); }
#define SSD1306_FlipScreenVertically()  { SSD1306_WriteCommand(SEGREMAP | 0x01); SSD1306_WriteCommand(COMSCANDEC); }
#define SSD1306_MirrorScreen()          { SSD1306_WriteCommand(SEGREMAP | 0x01); SSD1306_WriteCommand(COMSCANINC); }
#define SSD1306_MirrorFlipScreen()      { SSD1306_WriteCommand(SEGREMAP); SSD1306_WriteCommand(COMSCANDEC); }


uint8_t SSD1306_Init();
void    SSD1306_WriteCommand(uint8_t command);
void    SSD1306_UpdateScreen(uint8_t *displayBuff);
uint8_t SSD1306_tх_completed();



#endif

