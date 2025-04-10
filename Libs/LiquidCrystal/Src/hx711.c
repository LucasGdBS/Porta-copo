/*
 * hx711.c
 *
 *  Created on: Apr 10, 2025
 *      Author: lucas
 */

#include "hx711.h"

static float hx711_scale = 1.0f;
static int32_t hx711_offset = 0;

void HX711_Init(HX711* hx, GPIO_TypeDef* dout_port, uint16_t dout_pin,
                GPIO_TypeDef* sck_port, uint16_t sck_pin, uint8_t gain) {
    hx->dout_port = dout_port;
    hx->dout_pin = dout_pin;
    hx->sck_port = sck_port;
    hx->sck_pin = sck_pin;

    hx->gain = (gain == 128) ? 1 : (gain == 64) ? 3 : 2;

    HX711_Read(hx); // Faz leitura inicial para configurar ganho
}

uint8_t HX711_IsReady(HX711* hx) {
    return HAL_GPIO_ReadPin(hx->dout_port, hx->dout_pin) == GPIO_PIN_RESET;
}

void HX711_WaitReady(HX711* hx) {
    while (!HX711_IsReady(hx)) {
        HAL_Delay(1);
    }
}

int32_t HX711_Read(HX711* hx) {
    uint8_t i;
    int32_t data = 0;

    HX711_WaitReady(hx);

    __disable_irq();

    for (i = 0; i < 24; i++) {
        HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_SET);
        data = data << 1;

        if (HAL_GPIO_ReadPin(hx->dout_port, hx->dout_pin)) {
            data++;
        }

        HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_RESET);
    }

    for (i = 0; i < hx->gain; i++) {
        HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_RESET);
    }

    __enable_irq();

    // Conversão de 24 bits para 32 bits com sinal
    if (data & 0x800000) {
        data |= 0xFF000000;
    }

    return data;
}

int32_t HX711_ReadAverage(HX711* hx, uint8_t times) {
    int64_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += HX711_Read(hx);
    }
    return (int32_t)(sum / times);
}

void HX711_Tare(HX711* hx, uint8_t times) {
    int32_t val = HX711_ReadAverage(hx, times);
    HX711_SetOffset(val);
}

float HX711_GetUnits(HX711* hx, uint8_t times) {
    int32_t val = HX711_ReadAverage(hx, times) - hx711_offset;
    return (float)val / hx711_scale;
}

void HX711_SetScale(float scale) {
    hx711_scale = scale;
}

void HX711_SetOffset(int32_t offset) {
    hx711_offset = offset;
}

void HX711_PowerDown(HX711* hx) {
    HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

void HX711_PowerUp(HX711* hx) {
    HAL_GPIO_WritePin(hx->sck_port, hx->sck_pin, GPIO_PIN_RESET);
}





