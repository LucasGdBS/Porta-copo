/* Ref: https://github.com/vtx22/STM32-DS18B20/tree/master */

#ifndef DS18B20_H
#define DS18B20_H

// Inclui os headers corretos para o microcontrolador STM32
#if defined(STM32F0)
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_tim.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#elif defined(STM32F2)
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_tim.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_tim.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_tim.h"
#else
#error "Unsupported STM32 microcontroller. Make sure you build with -DSTM32F1 for example!"
#endif

#include <stdint.h> // Para tipos como uint8_t e uint16_t

// Estrutura para encapsular os dados do sensor DS18B20
typedef struct {
    TIM_HandleTypeDef *tim; // Ponteiro para o handle do timer
    GPIO_TypeDef *port;     // Ponteiro para a porta GPIO
    uint16_t pin;           // Número do pino GPIO
} DS18B20;

// Funções públicas para inicialização e leitura do sensor
void DS18B20_Init(DS18B20 *sensor, TIM_HandleTypeDef *tim, GPIO_TypeDef *port, uint16_t pin);

float DS18B20_ReadTempCelsius(DS18B20 *sensor);
float DS18B20_ReadTempFahrenheit(DS18B20 *sensor);

// Funções auxiliares para manipulação do pino de dados
void DS18B20_SetDataPin(DS18B20 *sensor, uint8_t on);
void DS18B20_ToggleDataPin(DS18B20 *sensor);
void DS18B20_SetPinOutput(DS18B20 *sensor);
void DS18B20_SetPinInput(DS18B20 *sensor);
GPIO_PinState DS18B20_ReadDataPin(DS18B20 *sensor);

// Funções para comunicação com o sensor
void DS18B20_StartSensor(DS18B20 *sensor);
void DS18B20_WriteData(DS18B20 *sensor, uint8_t data);
uint8_t DS18B20_ReadData(DS18B20 *sensor);

// Função para atraso em microssegundos
void DS18B20_DelayUs(DS18B20 *sensor, uint16_t us);

#endif // DS18B20_H