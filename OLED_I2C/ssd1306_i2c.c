
#include "ssd1306_i2c.h"
#include <linux/i2c.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h> // системный вызов функции i2c


uint8_t ssd1306_BufferTrnsit[SSD1306_BUFFER_SIZE];
volatile uint8_t ssd1306_updatestatus = 0;
volatile uint8_t ssd1306_updateend;
uint8_t i2c_command = 0;
int fd_i2c;


uint8_t SSD1306_Init() // возвращет 1 если ок
{

	if(wiringXSetup("milkv_duo", NULL) == -1) {
         wiringXGC();
         return 0;
     }

     if ((fd_i2c = wiringXI2CSetup(I2C_DEV, SSD1306_ADDRESS)) <0) {
         printf("I2C Setup failed: %i\n", fd_i2c);
         return 0;
     }

	  /* Init LCD */
	  SSD1306_WriteCommand(DISPLAYOFF);
	  SSD1306_WriteCommand(SETDISPLAYCLOCKDIV);
	  SSD1306_WriteCommand(0xF0); // Increase speed of the display max ~96Hz
	  SSD1306_WriteCommand(SETMULTIPLEX);
	  SSD1306_WriteCommand(SSD1306_HEIGHT - 1);
	  SSD1306_WriteCommand(SETDISPLAYOFFSET);
	  SSD1306_WriteCommand(0x00);
	  SSD1306_WriteCommand(SETSTARTLINE);
	  SSD1306_WriteCommand(CHARGEPUMP);
	  SSD1306_WriteCommand(0x14);
	  SSD1306_WriteCommand(MEMORYMODE);
	  SSD1306_WriteCommand(0x00);
	  SSD1306_WriteCommand(SEGREMAP);
	  SSD1306_WriteCommand(COMSCANINC);
	  SSD1306_WriteCommand(SETCOMPINS);

	  if (SSD1306_HEIGHT == 64)
	  {
	    SSD1306_WriteCommand(0x12);
	  }
	  else if (SSD1306_HEIGHT == 32)
	  {
	    SSD1306_WriteCommand(0x02);
	  }

	  SSD1306_WriteCommand(SETCONTRAST);

	  if (SSD1306_HEIGHT == 64)
	  {
	    SSD1306_WriteCommand(0xCF);
	  }
	  else if (SSD1306_HEIGHT == 32)
	  {
	    SSD1306_WriteCommand(0x8F);
	  }

	  SSD1306_WriteCommand(SETPRECHARGE);
	  SSD1306_WriteCommand(0xF1);
	  SSD1306_WriteCommand(SETVCOMDETECT); //0xDB, (additionally needed to lower the contrast)
	  SSD1306_WriteCommand(0x40);          //0x40 default, to lower the contrast, put 0
	  SSD1306_WriteCommand(DISPLAYALLON_RESUME);
	  SSD1306_WriteCommand(NORMALDISPLAY);
	  SSD1306_WriteCommand(0x2e);            // stop scroll
	  SSD1306_WriteCommand(DISPLAYON);

	  return 1;
}


void SSD1306_WriteCommand(uint8_t command)
{
	 // wiringXI2CWriteReg8(fd_i2c, 0x00, command);
     // HAL_I2C_Mem_Write(&SSD1306_I2C, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);
     union i2c_smbus_data data;
     data.byte = command;
     //i2c_smbus_access(fd_i2c, I2C_SMBUS_WRITE, 0x00, I2C_SMBUS_BYTE_DATA, &data);

	 struct i2c_smbus_ioctl_data args;
     args.read_write = I2C_SMBUS_WRITE;
     args.command = 0x00;
     args.size = I2C_SMBUS_BYTE_DATA;
     args.data = &data;
     ioctl(fd_i2c, I2C_SMBUS, &args);
}

void SSD1306_WriteData(uint8_t* data, uint16_t size)
{
    union i2c_smbus_data data2;
    uint16_t remaining = size;
    uint16_t offset = 0;

    while (remaining > 0) {
        // Определяем размер блока для текущей передачи
        uint16_t block_size = (remaining > I2C_SMBUS_BLOCK_MAX) ? I2C_SMBUS_BLOCK_MAX : remaining;

        // Устанавливаем размер блока и копируем данные в блок
        data2.block[0] = block_size; // Первый байт - размер блока
        memcpy(data2.block + 1, data + offset, block_size); // Копируем данные в блок

        // Настраиваем аргументы для ioctl
        struct i2c_smbus_ioctl_data args;
        args.read_write = I2C_SMBUS_WRITE; // Указываем, что это запись
        args.command = 0x40; // Команда для передачи данных
        args.size = I2C_SMBUS_I2C_BLOCK_BROKEN; // Тип передачи
        args.data = &data2; // Данные для передачи

        // Выполняем передачу данных через ioctl
        if (ioctl(fd_i2c, I2C_SMBUS, &args) < 0) {
            perror("I2C ioctl failed");
            return; // Обработка ошибки, если ioctl завершился неудачно
        }

        // Обновляем оставшийся размер и смещение
        remaining -= block_size; // Уменьшаем оставшийся размер
        offset += block_size; // Сдвигаем указатель на массив данных
    }
}

void SSD1306_UpdateScreen(uint8_t *displayBuff)
{
  // копируем в передающий буфер
  for(int i = 0; i < SSD1306_BUFFER_SIZE; i++) ssd1306_BufferTrnsit[i] = displayBuff[i];

  uint8_t i;
  for (i = 0; i < SSD1306_HEIGHT / 8; i++)
  {
	  SSD1306_WriteCommand(0xB0 + i);
#ifdef OLED_SH1106
	  SSD1306_WriteCommand(SETLOWCOLUMN + 2);
#else
	  SSD1306_WriteCommand(SETLOWCOLUMN );
#endif
	  SSD1306_WriteCommand(SETHIGHCOLUMN);
	  SSD1306_WriteData(&ssd1306_BufferTrnsit[SSD1306_WIDTH * i], SSD1306_WIDTH);
  }
}

uint8_t SSD1306_tх_completed() // затычка для совместимости
{
	return 0;
}
//***************


