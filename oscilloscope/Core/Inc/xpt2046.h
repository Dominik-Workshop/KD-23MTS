#ifndef _XPT2046_H_
#define _XPT2046_H_

#include "main.h"



#define	XPT2046_ACCURACY 	0
#define	XPT2046_REVERSED 	1
#define	XPT2046_MIRROR_X 	1
#define	XPT2046_MIRROR_Y 	1

#define RAW_MIN_X	300
#define RAW_MAX_X	3800
#define OUT_MIN_X	0
#define OUT_MAX_X	319

#define RAW_MIN_Y	400
#define RAW_MAX_Y	3750
#define OUT_MIN_Y	0
#define OUT_MAX_Y	479

#define	XPT2046_SPI 			hspi2
#define	XPT2046_NSS_SOFT	1
#define	XPT2046_NSS_PORT 	GPIOB
#define	XPT2046_NSS_PIN 	GPIO_PIN_12

#define	XPT2046_ADDR_I 	0x80
#define	XPT2046_ADDR_X 	0xD0
#define	XPT2046_ADDR_Y 	0x90

void XPT2046_Init(void);
uint16_t getRaw(uint8_t address);
uint16_t getX(void);
uint16_t getY(void);



#endif /* _XPT2046_H_ */
