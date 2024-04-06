/*
 * ILI9488.cpp
 *
 *  Created on: Apr 29, 2021
 *      Author: hydra
 */

#include "ILI9488.h"
#include "stdlib.h"
#include <stdbool.h>
#include <string.h>
extern SPI_HandleTypeDef hspi1;

extern DMA_HandleTypeDef hdma_spi1_tx;


extern uint8_t SPI1_TX_completed_flag;



uint8_t  tabcolor;
uint8_t mySPCR;

int16_t WIDTH;
int16_t HEIGHT;
 int16_t _width;
int16_t  _height;
int16_t cursor_x;
int16_t cursor_y;
uint16_t textcolor;
uint16_t textbgcolor;
uint8_t textsize_x;
uint8_t textsize_y;
 uint8_t rotation;


void ILI9488_Init(void)
{
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port,TFT_RST_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port,TFT_RST_Pin,GPIO_PIN_SET);
	_width=ILI9488_TFTWIDTH;
	_height=ILI9488_TFTHEIGHT;

	writecommand(0xE0);
	writedata(0x00);
	writedata(0x03);
	writedata(0x09);
	writedata(0x08);
	writedata(0x16);
	writedata(0x0A);
	writedata(0x3F);
	writedata(0x78);
	writedata(0x4C);
	writedata(0x09);
	writedata(0x0A);
	writedata(0x08);
	writedata(0x16);
	writedata(0x1A);
	writedata(0x0F);

	writecommand(0XE1);
	writedata(0x00);
	writedata(0x16);
	writedata(0x19);
	writedata(0x03);
	writedata(0x0F);
	writedata(0x05);
	writedata(0x32);
	writedata(0x45);
	writedata(0x46);
	writedata(0x04);
	writedata(0x0E);
	writedata(0x0D);
	writedata(0x35);
	writedata(0x37);
	writedata(0x0F);

	writecommand(0XC0);      //Power Control 1
	writedata(0x17);    //Vreg1out
	writedata(0x15);    //Verg2out

	writecommand(0xC1);      //Power Control 2
	writedata(0x41);    //VGH,VGL

	writecommand(0xC5);      //Power Control 3
	writedata(0x00);
	writedata(0x12);    //Vcom
	writedata(0x80);

	writecommand(0x36);      //Memory Access
	writedata(0x48);

	writecommand(0x36);      // Interface Pixel Format
	writedata(0x66); 	  //18 bit

	writecommand(0XB0);      // Interface Mode Control
	writedata(0x80);     			 //SDO NOT USE

	writecommand(0xB1);      //Frame rate
	writedata(0xA0);    //60Hz

	writecommand(0xB4);      //Display Inversion Control
	writedata(0x02);    //2-dot

	writecommand(0XB6); //Display Function Control  RGB/MCU Interface Control

	writedata(0x02);    //MCU
	writedata(0x02);    //Source,Gate scan dieection

	writecommand(0XE9);      // Set Image Functio
	writedata(0x00);    // Disable 24 bit data

	writecommand(0xF7);      // Adjust Control
	writedata(0xA9);
	writedata(0x51);
	writedata(0x2C);
	writedata(0x82);    // D7 stream, loose

	writecommand(ILI9488_SLPOUT);    //Exit Sleep

	HAL_Delay(120);

	writecommand(ILI9488_DISPON);    //Display on


}

void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	writecommand(ILI9488_CASET); // Column addr set
	writedata(x0 >> 8);
	writedata(x0 & 0xFF);     // XSTART
	writedata(x1 >> 8);
	writedata(x1 & 0xFF);     // XEND
	writecommand(ILI9488_PASET); // Row addr set
	writedata(y0 >> 8);
	writedata(y0 & 0xff);     // YSTART
	writedata(y1 >> 8);
	writedata(y1 & 0xff);     // YEND
	writecommand(ILI9488_RAMWR); // write to RAM
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
}
void setScrollArea(uint16_t topFixedArea, uint16_t bottomFixedArea)
{
	writecommand(0x33); // Vertical scroll definition
	writedata(topFixedArea >> 8);
	writedata(topFixedArea);
	writedata((_height - topFixedArea - bottomFixedArea) >> 8);
	writedata(_height - topFixedArea - bottomFixedArea);
	writedata(bottomFixedArea >> 8);
	writedata(bottomFixedArea);
}
void scroll(uint16_t pixels)
{
	writecommand(0x37); // Vertical scrolling start address
	writedata(pixels >> 8);
	writedata(pixels);
}
void pushColor(uint16_t color)
{
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);
	write16BitColor(color);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);
}
void pushColors(uint16_t *data, uint8_t len, uint8_t first)
{
	uint16_t color;
	uint8_t buff[len * 3 + 1];
	uint16_t count = 0;
	uint8_t lencount = len;


	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);
	if (first == 1) {
		HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	}
	while (lencount--)
	{
		color = *data++;
		buff[count] = (((color & 0xF800) >> 11) * 255) / 31;
		count++;
		buff[count] = (((color & 0x07E0) >> 5) * 255) / 63;
		count++;
		buff[count] = ((color & 0x001F) * 255) / 31;
		count++;
	}
	HAL_SPI_Transmit(&hspi1, buff, len * 3, 100);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);

}
void drawImage(const uint8_t* img, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{

	if ((x >= _width) || (y >= _height))
		return;
	if ((x + w - 1) >= _width)
		w = _width - x;
	if ((y + h - 1) >= _height)
		h = _height - y;
	setAddrWindow(x, y, x + w - 1, y + h - 1);
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);

	uint8_t linebuff[w * 3 + 1];
	uint32_t count = 0;
	for (uint16_t i = 0; i < h; i++) {
		uint16_t pixcount = 0;
		for (uint16_t o = 0; o < w; o++) {
			uint8_t b1 = img[count];
			count++;
			uint8_t b2 = img[count];
			count++;
			uint16_t color = b1 << 8 | b2;
			linebuff[pixcount] = (((color & 0xF800) >> 11) * 255)
					/ 31;
			pixcount++;
			linebuff[pixcount] = (((color & 0x07E0) >> 5) * 255)
					/ 63;
			pixcount++;
			linebuff[pixcount] = ((color & 0x001F) * 255) / 31;
			pixcount++;
		}
//		HAL_SPI_Transmit(_spi->getHandler(), linebuff, w * 3, 100);
		HAL_SPI_Transmit(&hspi1, linebuff, w * 3, 100);

	}

	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);
}
void fillScreen(uint16_t color)
{
	fillRect(0, 0,  _width, _height, color);
}
void drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
		return;

	setAddrWindow(x, y, x + 1, y + 1);
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);
	write16BitColor(color);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);

}
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{

	if ((x >= _width) || (y >= _height))
		return;

	if ((y + h - 1) >= _height)
		h = _height - y;

	setAddrWindow(x, y, x, y + h - 1);
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);

	while (h--) {
		write16BitColor(color);
	}
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);

}
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{

	if ((x >= _width) || (y >= _height))
		return;
	if ((x + w - 1) >= _width)
		w = _width - x;
	setAddrWindow(x, y, x + w - 1, y);
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);
	while (w--)
	{
		write16BitColor(color);
	}
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);
}
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color)
{
	 if (x0 == x1) {
	    if (y0 > y1)
	      _swap_int16_t(y0, y1);
	    drawFastVLine(x0, y0, y1 - y0 + 1, color);
	  } else if (y0 == y1) {
	    if (x0 > x1)
	      _swap_int16_t(x0, x1);
	    drawFastHLine(x0, y0, x1 - x0 + 1, color);
	  } else {

	    writeLine(x0, y0, x1, y1, color);

	  }

}
void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color) {

  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0 <= x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	if ((x >= _width) || (y >= _height))
		return;
	if ((x + w - 1) >= _width)
		w = _width - x;
	if ((y + h - 1) >= _height)
		h = _height - y;

	setAddrWindow(x, y, x + w - 1, y + h - 1);

	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);

	for (y = h; y > 0; y--) {
		for (x = w; x > 0; x--) {

			write16BitColor(color);
		}
	}

	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);

	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);

//		for (y = h; y > 0; y--) {
//			for (x = w; x > 0; x--) {
//
//				write16BitColor(ILI9488_DARKGREY);
//			}
//		}

//		HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);

}
void setRotation(uint8_t r)
{

	writecommand(ILI9488_MADCTL);
	rotation = r % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		writedata(MADCTL_MX | MADCTL_BGR);
		_width = ILI9488_TFTWIDTH;
		_height = ILI9488_TFTHEIGHT;
		break;
	case 1:
		writedata(MADCTL_MV | MADCTL_BGR);
		_width = ILI9488_TFTHEIGHT;
		_height = ILI9488_TFTWIDTH;
		break;
	case 2:
		writedata(MADCTL_MY | MADCTL_BGR);
		_width = ILI9488_TFTWIDTH;
		_height = ILI9488_TFTHEIGHT;
		break;
	case 3:
		writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		_width = ILI9488_TFTHEIGHT;
		_height = ILI9488_TFTWIDTH;
		break;
	}

}
void invertDisplay(uint8_t i)
{

	  writecommand(i ? ILI9488_INVON : ILI9488_INVOFF);

}
uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
void spiwrite(uint8_t data)
{
	HAL_SPI_Transmit(&hspi1, &data, 1, 1);

}
void writecommand(uint8_t c)
{

	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_RESET);


	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, &c, 1, 1);


	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);

}
void write16BitColor(uint16_t color)
{

	  uint8_t r = (color & 0xF800) >> 11;
	  uint8_t g = (color & 0x07E0) >> 5;
	  uint8_t b = color & 0x001F;

	  r = (r * 255) / 31;
	  g = (g * 255) / 63;
	  b = (b * 255) / 31;


	  HAL_SPI_Transmit(&hspi1, &r, 1, 1);
	  HAL_SPI_Transmit(&hspi1, &g, 1, 1);
	  HAL_SPI_Transmit(&hspi1, &b, 1, 1);


}
void writedata(uint8_t d)
{


	HAL_GPIO_WritePin(TFT_DC_GPIO_Port,TFT_DC_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_RESET);

	HAL_SPI_Transmit(&hspi1, &d, 1, 1);
	//HAL_SPI_Transmit(_spi.getHandler(), &tmp,1,100);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port,TFT_CS_Pin,GPIO_PIN_SET);

}
void testLines(uint8_t color)
{

	//unsigned long start, t;
	int x1, y1, x2, y2, w = _width, h = _height;
	fillScreen(ILI9488_BLACK);

	x1 = y1 = 0;
	y2 = h - 1;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);
	fillScreen(ILI9488_BLACK);

	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);

	fillScreen(ILI9488_BLACK);

	x1 = 0;
	y1 = h - 1;
	y2 = 0;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);

	fillScreen(ILI9488_BLACK);

	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;

	for (x2 = 0; x2 < w; x2 += 6)
		drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for (y2 = 0; y2 < h; y2 += 6)
		drawLine(x1, y1, x2, y2, color);
}
void sendasIO(uint8_t d)
{
	for (uint8_t bit = 0x80; bit; bit >>= 1) {

		if (d & bit) {
			//_mosi->setHigh();
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		} else {
			//_mosi->setLow();
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		}
		//_sck->setHigh();
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		//HAL_Delay(4);
		//__ASM volatile ("NOP");

		//_sck->setLow();
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}
/*void ILI9488::commandList(uint8_t *addr)
 {
	uint8_t numCommands, numArgs;
	uint16_t ms;

	numCommands = pgm_read_byte(addr++);
	while (numCommands--) {
		writecommand(pgm_read_byte(addr++));
		numArgs = pgm_read_byte(addr++);
		ms = numArgs & 0x80;
		numArgs &= ~0x80;
		while (numArgs--) {
			writedata(pgm_read_byte(addr++));
		}

		if (ms) {
			ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
			if (ms == 255)
				ms = 500;     // If 255, delay for 500 ms
			delay(ms);
		}
	}

}*/


void ILI9341_Draw_Colour(uint16_t Colour)
{
//SENDS COLOUR



	  uint8_t r = (Colour & 0xF800) >> 11;
	  uint8_t g = (Colour & 0x07E0) >> 5;
	  uint8_t b = Colour & 0x001F;

	  r = (r * 255) / 31;
	  g = (g * 255) / 63;
	  b = (b * 255) / 31;

		unsigned char TempBuffer[3] =
		{ r, g ,b };

		for (uint32_t j = 0; j < 480*320; j++)
		{
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

	SPI1_TX_completed_flag = 0;
	HAL_SPI_Transmit_DMA(&hspi1, TempBuffer, 3);
	while (SPI1_TX_completed_flag == 0)

	HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
		}
}

//INTERNAL FUNCTION OF LIBRARY
/*Sends block colour information to LCD*/
void ILI9341_Draw_Colour_Burst(uint16_t Colour, uint32_t Size)
{
//SENDS COLOUR
	uint32_t Buffer_Size = 0;
	if ((Size * 2) < BURST_MAX_SIZE)
	{
		Buffer_Size = Size;
	}
	else
	{
		Buffer_Size = BURST_MAX_SIZE;
	}

	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET);

//	unsigned char MSB_color = Colour >> 8;

	  uint8_t r = (Colour & 0xF800) >> 11;
	  uint8_t g = (Colour & 0x07E0) >> 5;
	  uint8_t b = Colour & 0x001F;

	  r = (r * 255) / 31;
	  g = (g * 255) / 63;
	  b = (b * 255) / 31;

	unsigned char burst_buffer[Buffer_Size];
	for (uint32_t j = 0; j < Buffer_Size; j += 3)
	{
		burst_buffer[j] = r;
		burst_buffer[j + 1] = g;
		burst_buffer[j + 2] = b;
	}

	uint32_t Sending_Size = Size * 3;
	uint32_t Sending_in_Block = Sending_Size / Buffer_Size;
	uint32_t Remainder_from_block = Sending_Size % Buffer_Size;

	if (Sending_in_Block != 0)
	{
		for (uint32_t j = 0; j < (Sending_in_Block); j++)
		{
			SPI1_TX_completed_flag = 0;
			HAL_SPI_Transmit_DMA(&hspi1, (unsigned char*) burst_buffer, Buffer_Size);
			while (SPI1_TX_completed_flag == 0);
		}
	}

//REMAINDER!

	if (Remainder_from_block > 0)
	{
		SPI1_TX_completed_flag = 0;
		HAL_SPI_Transmit_DMA(&hspi1, (unsigned char*) burst_buffer, Remainder_from_block);
		while (SPI1_TX_completed_flag == 0);
	}
	HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET);
}

//FILL THE ENTIRE SCREEN WITH SELECTED COLOUR (either #define-d ones or custom 16bit)
/*Sets address (entire screen) and Sends Height*Width ammount of colour information to LCD*/
void ILI9341_Fill_Screen(uint16_t Colour)
{
//	LCD_Address_Set(xsta,ysta+OFFSET_Y,xend-1,yend-1+OFFSET_Y);/
	setAddrWindow(0, 0, ILI9488_TFTHEIGHT-1, ILI9488_TFTWIDTH-1);
	ILI9341_Draw_Colour_Burst(Colour, ILI9488_TFTWIDTH * ILI9488_TFTHEIGHT);
}


void ILI9341_Fill_Screen1(uint16_t x, uint16_t y, uint16_t w, uint16_t h,uint16_t Colour)
{
//	LCD_Address_Set(xsta,ysta+OFFSET_Y,xend-1,yend-1+OFFSET_Y);/
	setAddrWindow(x, y, h-1, w-1);
	ILI9341_Draw_Colour_Burst(Colour, w * h);
}

//static void WriteDataDMA(const void *data, uint16_t length)
//{
//	txComplete = false;
//	HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)data, length);
//}
//
//static void WaitForDMAWriteComplete(void)
//{
//	while (txComplete == false)
//	{
//	}
//}
//
//void ILI9341FilledRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, colour_t colour)
//{
//	colour_t dmaBuffer[DMA_BUFFER_SIZE];
//	uint8_t i;
//	uint32_t totalBytesToWrite;
//	uint32_t bytesToWriteThisTime;
//
//	for (i = 0U; i < DMA_BUFFER_SIZE; i++)
//	{
//		dmaBuffer[i] = __builtin_bswap16(colour);
//	}
//
//	setAddrWindow(x, y, x + ILI9488_TFTWIDTH - 1U, y + ILI9488_TFTHEIGHT - 1U);
//	totalBytesToWrite = (uint32_t)width * (uint32_t)height * (uint32_t)sizeof(colour_t);
//	bytesToWriteThisTime = DMA_BUFFER_SIZE * (uint16_t)sizeof(colour_t);
//
//	while (totalBytesToWrite > 0UL)
//	{
//		if (totalBytesToWrite < bytesToWriteThisTime)
//		{
//			bytesToWriteThisTime = totalBytesToWrite;
//		}
//		totalBytesToWrite -= bytesToWriteThisTime;
//
//		WriteDataDMA(&dmaBuffer, bytesToWriteThisTime);
//		WaitForDMAWriteComplete();
//	}
//}
//
//static void filledRectangleClip(int16_t x, int16_t y, uint16_t width, uint16_t height, colour_t colour)
//{
//	int16_t x_end;
//	int16_t y_end;
//
//	if (x > (int16_t)ILI9488_TFTWIDTH - 1 || y > (int16_t)ILI9488_TFTHEIGHT - 1)
//	{
//		return;
//	}
//
//	if (x + (int16_t)width < 0 || y + (int16_t)height < 0)
//	{
//		return;
//	}
//
//	if (x < 0)
//	{
//		width -= (uint16_t)-x;
//		x = 0;
//	}
//
//	if (y < 0)
//	{
//		height -= (uint16_t)-y;
//		y = 0;
//	}
//
//	x_end = x + (int16_t)width - 1;
//	if (x_end >(int16_t)ILI9488_TFTWIDTH - 1)
//	{
//		x_end = (int16_t)ILI9488_TFTWIDTH - 1;
//	}
//
//	y_end = y + (int16_t)height - 1;
//	if (y_end > (int16_t)ILI9488_TFTHEIGHT - 1)
//	{
//		y_end = (int16_t)ILI9488_TFTHEIGHT - 1;
//	}
//
//	// clipped sizes
//	width = (uint16_t)(x_end - x + 1);
//	height = (uint16_t)(y_end - y + 1);
//
//	ILI9341FilledRectangle((uint16_t)x, (uint16_t)y, width, height, colour);
//}
//
//
//void GraphicsClear(colour_t colour)
//{
//	filledRectangleClip(0U, 0U, (uint16_t)ILI9488_TFTWIDTH, (uint16_t)ILI9488_TFTHEIGHT, colour);
//}
//



void LCD_Char(int16_t x, int16_t y, const GFXglyph *glyph, const GFXfont *font, uint8_t size, uint32_t color24)
{
	uint8_t  *bitmap = font -> bitmap;
	uint16_t bo = glyph -> bitmapOffset;
	uint8_t bits = 0, bit = 0;
	uint16_t set_pixels = 0;
	uint8_t  cur_x, cur_y;
	for (cur_y = 0; cur_y < glyph -> height; cur_y++)
	{
		for (cur_x = 0; cur_x < glyph -> width; cur_x++)
		{
			if (bit == 0)
			{
				bits = (*(const unsigned char *)(&bitmap[bo++]));
				bit  = 0x80;
			}
			if (bits & bit) set_pixels++;
			else if (set_pixels > 0)
			{
				fillRect(x + (glyph -> xOffset + cur_x - set_pixels) * size, y + (glyph -> yOffset + cur_y) * size, size * set_pixels, size, color24);
				set_pixels = 0;
			}
			bit >>= 1;
		}
		if (set_pixels > 0)
		{
			fillRect(x + (glyph -> xOffset + cur_x-set_pixels) * size, y + (glyph -> yOffset + cur_y) * size, size * set_pixels, size, color24);
			set_pixels = 0;
		}
	}
}

void LCD_Font(uint16_t x, uint16_t y, const char *text, const GFXfont *p_font, uint8_t size, uint32_t color24)
{
	int16_t cursor_x = x;
	int16_t cursor_y = y;
	GFXfont font;
	memcpy(&font, p_font, sizeof(GFXfont));
	for (uint16_t text_pos = 0; text_pos < strlen(text); text_pos++)
	{
		char c = text[text_pos];
		if (c == '\n')
		{
			cursor_x = x;
			cursor_y += font.yAdvance * size;
		}
		else if (c >= font.first && c <= font.last && c != '\r')
		{
			GFXglyph glyph;
			memcpy(&glyph, &font.glyph[c - font.first], sizeof(GFXglyph));
			LCD_Char(cursor_x, cursor_y, &glyph, &font, size, color24);
			cursor_x += glyph.xAdvance * size;
		}
	}
}

