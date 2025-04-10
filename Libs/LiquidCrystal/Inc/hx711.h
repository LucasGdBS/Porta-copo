/*
 * hx711.h
 *
 *  Created on: Apr 10, 2025
 *      Author: lucas
 */

#ifndef HX711_H_
#define HX711_H_

#include "stm32l4xx_hal.h"// Altere de acordo com sua série STM32

typedef struct {
    GPIO_TypeDef* dout_port;
    uint16_t dout_pin;

    GPIO_TypeDef* sck_port;
    uint16_t sck_pin;

    uint8_t gain; // 128, 64 ou 32
} HX711;

void HX711_Init(HX711* hx, GPIO_TypeDef* dout_port, uint16_t dout_pin,
                GPIO_TypeDef* sck_port, uint16_t sck_pin, uint8_t gain);
uint8_t HX711_IsReady(HX711* hx);
void HX711_WaitReady(HX711* hx);
int32_t HX711_Read(HX711* hx);
int32_t HX711_ReadAverage(HX711* hx, uint8_t times);
void HX711_Tare(HX711* hx, uint8_t times);
float HX711_GetUnits(HX711* hx, uint8_t times);
void HX711_SetScale(float scale);
void HX711_SetOffset(int32_t offset);
void HX711_PowerDown(HX711* hx);
void HX711_PowerUp(HX711* hx);



#endif /* HX711_H_ */
