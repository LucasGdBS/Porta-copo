#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f1xx_hal.h"  // Altere conforme o seu STM32

typedef struct
{
    TIM_HandleTypeDef *htim;
    GPIO_TypeDef *port;
    uint16_t pin;
} DS18B20;

void DS18B20_Init(DS18B20 *sensor, TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin);

float DS18B20_ReadTempCelsius(DS18B20 *sensor);
float DS18B20_ReadTempFahrenheit(DS18B20 *sensor);

#endif // DS18B20_H

