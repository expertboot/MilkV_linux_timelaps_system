
#include "screen_graphics.h"

//#define OLED_UPDATE()     sh1106_SPI_Write_arry(GRAPHICS_Buffer, SH1106_BUFFER_SIZE)

#define ABS(x)   ((x) > 0 ? (x) : -(x))

#define OLED_BUFFER_SIZE (GRAPHICS_WIDTH  * GRAPHICS_HEIGHT / 8)

uint8_t GRAPHICS_Buffer[OLED_BUFFER_SIZE];

/* Private SSD1306 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} GRAPHICS_t;

/* Private variable */
static GRAPHICS_t GRAPHICS;

//*************************************** Функции берем из библиотеки экрана ******************
void GRAPHICS_UpdateScreen(void)
{
#ifdef DISPLAY_SH1106_SPI
	if( sh1106_SPI_tх_completed() == 1) return; // если экран занят
	sh1106_update_screen(GRAPHICS_Buffer, OLED_BUFFER_SIZE);
#endif
#ifdef DISPLAY_SSD1306
	if(SSD1306_tх_completed() == 1) return; // если экран занят
	SSD1306_UpdateScreen(GRAPHICS_Buffer);
#endif
}

GRAPHICS_BUSY_UPDATE_t GRAPHICS_isBusy(void) // 1 - занят
{
#ifdef DISPLAY_SH1106_SPI
	if( sh1106_SPI_tх_completed() == 1) return GRAPHICS_BUSY;
	else return GRAPHICS_REDY;
#endif
#ifdef DISPLAY_SSD1306
	return  SSD1306_tх_completed();
#endif
}

uint8_t GRAPHICS_Init(void) // возвращет 1 если ок
{
#ifdef DISPLAY_SH1106_SPI
    sh1106_SPI_Init(GRAPHICS_WIDTH, GRAPHICS_HEIGHT);
	GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
	GRAPHICS_UpdateScreen();
	GRAPHICS.CurrentX = 0;
	GRAPHICS.CurrentY = 0;
	GRAPHICS.Initialized = 1;
    return 1;
#endif
#ifdef DISPLAY_SSD1306
	uint8_t status = SSD1306_Init();
	if(status == 1)
	{
		GRAPHICS_Fill(GRAPHICS_COLOR_BLACK);
		GRAPHICS_UpdateScreen();
		GRAPHICS.CurrentX = 0;
		GRAPHICS.CurrentY = 0;
		GRAPHICS.Initialized = 1;
		return 1;
	}
	return 0;
#endif
}
//******************************************************************************************

void GRAPHICS_ToggleInvert(void) {
	uint16_t i;
	/* Toggle invert */
	GRAPHICS.Inverted = !GRAPHICS.Inverted;
	/* Do memory toggle */
	for (i = 0; i < sizeof(GRAPHICS_Buffer); i++) {
		GRAPHICS_Buffer[i] = ~GRAPHICS_Buffer[i];
	}
}

void GRAPHICS_Fill(GRAPHICS_COLOR_t color) {
	/* Set memory */
	memset(GRAPHICS_Buffer, (color == GRAPHICS_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(GRAPHICS_Buffer));
}

void GRAPHICS_DrawPixel(uint16_t x, uint16_t y, GRAPHICS_COLOR_t color) {
	if (
		x >= GRAPHICS_WIDTH ||
		y >= GRAPHICS_HEIGHT
	) {
		/* Error */
		return;
	}

	/* Check if pixels are inverted */
	if (GRAPHICS.Inverted) {
		color = (GRAPHICS_COLOR_t)!color;
	}
	/* Set color */
	if (color == GRAPHICS_COLOR_WHITE)
	{
		uint16_t point = x + (y / 8) * GRAPHICS_WIDTH;
		GRAPHICS_Buffer[point] |= 1 << (y % 8);
	}

	if (color == GRAPHICS_COLOR_BLACK)  GRAPHICS_Buffer[x + (y / 8) * GRAPHICS_WIDTH] &= ~(1 << (y % 8));
	if (color == GRAPHICS_COLOR_INVERT) GRAPHICS_Buffer[x + (y / 8) * GRAPHICS_WIDTH] ^= 1 << (y % 8);
}

void GRAPHICS_GotoXY(uint16_t x, uint16_t y) {
	/* Set write pointers */
	GRAPHICS.CurrentX = x;
	GRAPHICS.CurrentY = y;
}

uint8_t GRAPHICS_getY(void) {

	return GRAPHICS.CurrentY;
}

uint8_t GRAPHICS_getX(void) {

	return GRAPHICS.CurrentX;
}


char GRAPHICS_Putc(char ch, FontDef_t* Font, GRAPHICS_COLOR_t color) {
	uint32_t i, b, j;
	uint8_t  cntByte;
	uint32_t num;
	uint8_t  length;
	uint8_t  a;
	const uint8_t *data;
	/* Check available space in LCD */
	if (
		GRAPHICS_WIDTH <= (GRAPHICS.CurrentX + Font->FontWidth) ||
		GRAPHICS_HEIGHT <= (GRAPHICS.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}
    // узнаем количество байт в столбце символа
	if(Font->FontHeight <= 8)  cntByte = 1; else
	if(Font->FontHeight <= 16) cntByte = 2; else
	if(Font->FontHeight <= 24) cntByte = 3;

#ifdef FONT_RUSSIAN
	if(ch < 0xC0){ a = 0x20; data = Font->data;   } // тип алфавита, английский или русский
	else         { a = 0xC0; data = Font->dataRus;}
#else
	a = 0x20; data = Font->data;
#endif
    length = data[(ch - a) * ((Font->FontWidth * cntByte)+1)]; // ширина символа

	for (i = 0; i < length; i++)
	{
		num = (ch - a) * ((Font->FontWidth * cntByte)+1) + (i * cntByte) + 1; // выбираем элемент масива, она обозначает строку символа в масиве
		b = data[num];
		if(cntByte >= 2) b |=  data[num + 1] << 8 ;
		if(cntByte >= 3) b |=  data[num + 2] << 16 ;
		//отрисовка столбика символа
		for (j = 0; j < Font->FontHeight; j++) // проходимся по вертикали
		{
			if (b  & (1 << j))
				GRAPHICS_DrawPixel(GRAPHICS.CurrentX + i, (GRAPHICS.CurrentY + j), (GRAPHICS_COLOR_t) color);
			else
				GRAPHICS_DrawPixel(GRAPHICS.CurrentX + i, (GRAPHICS.CurrentY + j), (GRAPHICS_COLOR_t)!color);
		}
	}
	GRAPHICS.CurrentX += length;

	// рисуем межсимвольное пространство
	for (i = 0; i < Font->Spice; i++)
	for (j = 0; j < Font->FontHeight; j++) // проходимся по вертикали
		GRAPHICS_DrawPixel(GRAPHICS.CurrentX + i, (GRAPHICS.CurrentY + j), (GRAPHICS_COLOR_t)!color);

	/* Increase pointer */
	GRAPHICS.CurrentX += Font->Spice;
	/* Return character written */
	return ch;
}


// возвращает длину символа в пикселях
uint8_t GRAPHICS_lengtchPixChar(char ch, FontDef_t* Font)
{
	 // узнаем количество байт в столбце символа
	    uint8_t cntByte;
	    const uint8_t *data;
	    uint8_t a;
		if(Font->FontHeight <= 8)  cntByte = 1; else
		if(Font->FontHeight <= 16) cntByte = 2; else
		if(Font->FontHeight <= 24) cntByte = 3;

#ifdef FONT_RUSSIAN
	if(ch < 0xC0){ a = 0x20; data = Font->data;   } // тип алфавита, английский или русский
	else         { a = 0xC0; data = Font->dataRus;}
#else
	a = 0x20; data = Font->data;
#endif
// в первом байте символа лежит его длинна
		return  data[(ch - a) * ((Font->FontWidth * cntByte) + 1)] + Font->Spice;
}

// возвращает длину строки в пикселях
uint8_t GRAPHICS_lengtchPixStroke(char *strok, FontDef_t* Font)
{
	 // узнаем количество байт в столбце символа
	    uint8_t cntByte;
	    uint8_t length = 0;
	    uint8_t size = strlen(strok);
	    const uint8_t *data;
	    uint8_t a;

		if(Font->FontHeight <= 8)  cntByte = 1; else
		if(Font->FontHeight <= 16) cntByte = 2; else
		if(Font->FontHeight <= 24) cntByte = 3;

		for (int i = 0; i < size; i++)
		{
#ifdef FONT_RUSSIAN
	if(strok[i] < 0xC0){ a = 0x20; data = Font->data;   } // тип алфавита, английский или русский
	else               { a = 0xC0; data = Font->dataRus;}
#else
	a = 0x20; data = Font->data;
#endif

			length += data[(strok[i] - a) * ((Font->FontWidth * cntByte)+1)]; // ширина символа
			length += Font->Spice;

		}
		return length;
}


char GRAPHICS_Puts(char* str, FontDef_t* Font, GRAPHICS_COLOR_t color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (GRAPHICS_Putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}
		/* Increase string pointer */
		str++;
	}
	/* Everything OK, zero should be returned */
	return *str;
}


void GRAPHICS_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, GRAPHICS_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= GRAPHICS_WIDTH) {
		x0 = GRAPHICS_WIDTH - 1;
	}
	if (x1 >= GRAPHICS_WIDTH) {
		x1 = GRAPHICS_WIDTH - 1;
	}
	if (y0 >= GRAPHICS_HEIGHT) {
		y0 = GRAPHICS_HEIGHT - 1;
	}
	if (y1 >= GRAPHICS_HEIGHT) {
		y1 = GRAPHICS_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			GRAPHICS_DrawPixel(x0, i, c);
		}

		/* Return from function */
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			GRAPHICS_DrawPixel(i, y0, c);
		}

		/* Return from function */
		return;
	}

	while (1) {
		GRAPHICS_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}
}

// x, y, - верхний левый угол
// w, h - длинна, высота вниз
void GRAPHICS_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, GRAPHICS_COLOR_t c) {
	/* Check input parameters */
	if (
		x >= GRAPHICS_WIDTH ||
		y >= GRAPHICS_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= GRAPHICS_WIDTH) {
		w = GRAPHICS_WIDTH - x;
	}
	if ((y + h) >= GRAPHICS_HEIGHT) {
		h = GRAPHICS_HEIGHT - y;
	}

	/* Draw 4 lines */
	GRAPHICS_DrawLine(x, y, x + w, y, c);         /* Top line */
	GRAPHICS_DrawLine(x, y + h, x + w, y + h, c); /* Bottom line */
	GRAPHICS_DrawLine(x, y, x, y + h, c);         /* Left line */
	GRAPHICS_DrawLine(x + w, y, x + w, y + h, c); /* Right line */
}

void GRAPHICS_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, GRAPHICS_COLOR_t c) {
	uint8_t i;

	/* Check input parameters */
	if (
		x >= GRAPHICS_WIDTH ||
		y >= GRAPHICS_HEIGHT
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= GRAPHICS_WIDTH) {
		w = GRAPHICS_WIDTH - x;
	}
	if ((y + h) >= GRAPHICS_HEIGHT) {
		h = GRAPHICS_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		GRAPHICS_DrawLine(x, y + i, x + w, y + i, c);
	}
}

void GRAPHICS_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, GRAPHICS_COLOR_t color) {
	/* Draw lines */
	GRAPHICS_DrawLine(x1, y1, x2, y2, color);
	GRAPHICS_DrawLine(x2, y2, x3, y3, color);
	GRAPHICS_DrawLine(x3, y3, x1, y1, color);
}


void GRAPHICS_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, GRAPHICS_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
	curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		GRAPHICS_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void GRAPHICS_DrawCircle(int16_t x0, int16_t y0, int16_t r, GRAPHICS_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    GRAPHICS_DrawPixel(x0, y0 + r, c);
    GRAPHICS_DrawPixel(x0, y0 - r, c);
    GRAPHICS_DrawPixel(x0 + r, y0, c);
    GRAPHICS_DrawPixel(x0 - r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        GRAPHICS_DrawPixel(x0 + x, y0 + y, c);
        GRAPHICS_DrawPixel(x0 - x, y0 + y, c);
        GRAPHICS_DrawPixel(x0 + x, y0 - y, c);
        GRAPHICS_DrawPixel(x0 - x, y0 - y, c);

        GRAPHICS_DrawPixel(x0 + y, y0 + x, c);
        GRAPHICS_DrawPixel(x0 - y, y0 + x, c);
        GRAPHICS_DrawPixel(x0 + y, y0 - x, c);
        GRAPHICS_DrawPixel(x0 - y, y0 - x, c);
    }
}

void GRAPHICS_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, GRAPHICS_COLOR_t c) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    GRAPHICS_DrawPixel(x0, y0 + r, c);
    GRAPHICS_DrawPixel(x0, y0 - r, c);
    GRAPHICS_DrawPixel(x0 + r, y0, c);
    GRAPHICS_DrawPixel(x0 - r, y0, c);
    GRAPHICS_DrawLine(x0 - r, y0, x0 + r, y0, c);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        GRAPHICS_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, c);
        GRAPHICS_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, c);

        GRAPHICS_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, c);
        GRAPHICS_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, c);
    }
}

// вывод строки с ограничением по длинне пикселей, остальная часть строки не выводится
char GRAPHICS_PutsToLimit(char* str, int8_t x_limitPix, FontDef_t* Font, GRAPHICS_COLOR_t color) {

	if(x_limitPix < Font->FontWidth) return *str; // если ширина символа меньше лимита то выходим
	uint8_t sumPix = 0;

	while (*str) {
		sumPix += GRAPHICS_lengtchPixChar(*str, Font);
		if(sumPix > x_limitPix) return *str;
		if (GRAPHICS_Putc(*str, Font, color) != *str) return *str; /* Return error */
		str++; /* Increase string pointer */
	}
	return *str; 	/* Everything OK, zero should be returned */
}

// печать строки по центру экрана
// y - кордината строки по вертикали
void GRAPHICS_centerLine_Puts(uint8_t y_pix, char * stroke, FontDef_t *font , GRAPHICS_COLOR_t color)
{
	uint8_t pixLen  = GRAPHICS_lengtchPixStroke(stroke, font); // узнаем длинну строки
	pixLen = (GRAPHICS_WIDTH - pixLen) / 2;
	GRAPHICS_GotoXY(pixLen, y_pix);
	GRAPHICS_Puts(stroke, font, color);
}

void GRAPHICS_ON(void) {

}
void GRAPHICS_OFF(void) {

}
