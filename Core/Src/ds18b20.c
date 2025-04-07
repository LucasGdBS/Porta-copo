/* Ref: https://github.com/vtx22/STM32-DS18B20/tree/master */

#include "ds18b20.h"
#include <stdbool.h> // Para usar o tipo bool

/*
Inicializa o sensor de temperatura
@param sensor Ponteiro para a estrutura DS18B20
@param tim Ponteiro para o handle do timer de hardware
@param port Porta GPIO do pino do sensor
@param pin Número do pino GPIO do sensor
*/
void DS18B20_Init(DS18B20 *sensor, TIM_HandleTypeDef *tim, GPIO_TypeDef *port, uint16_t pin)
{
    sensor->tim = tim;
    sensor->port = port;
    sensor->pin = pin;

    // Verifica se o timer foi inicializado corretamente
    if (HAL_TIM_Base_Start(sensor->tim) != HAL_OK)
    {
        // Adicione um tratamento de erro aqui, se necessário
        return;
    }
}

/*
Aguarda por um tempo em microssegundos usando o contador do timer
*/
void DS18B20_DelayUs(DS18B20 *sensor, uint16_t us)
{
    sensor->tim->Instance->CNT = 0; // Zera o contador do timer
    while (sensor->tim->Instance->CNT < us)
        ; // Aguarda até atingir o tempo desejado
}

void DS18B20_SetDataPin(DS18B20 *sensor, uint8_t on)
{
    HAL_GPIO_WritePin(sensor->port, sensor->pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void DS18B20_ToggleDataPin(DS18B20 *sensor)
{
    HAL_GPIO_TogglePin(sensor->port, sensor->pin);
}

GPIO_PinState DS18B20_ReadDataPin(DS18B20 *sensor)
{
    return HAL_GPIO_ReadPin(sensor->port, sensor->pin);
}

void DS18B20_SetPinOutput(DS18B20 *sensor)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = sensor->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(sensor->port, &GPIO_InitStruct);
}

void DS18B20_SetPinInput(DS18B20 *sensor)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = sensor->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(sensor->port, &GPIO_InitStruct);
}

void DS18B20_StartSensor(DS18B20 *sensor)
{
    DS18B20_SetPinOutput(sensor);
    DS18B20_SetDataPin(sensor, 0);

    DS18B20_DelayUs(sensor, 480);
    DS18B20_SetPinInput(sensor);
    DS18B20_DelayUs(sensor, 80);

    // Aguarda o tempo restante
    DS18B20_DelayUs(sensor, 400);
}

void DS18B20_WriteData(DS18B20 *sensor, uint8_t data)
{
    DS18B20_SetPinOutput(sensor);

    for (uint8_t i = 0; i < 8; i++)
    {
        if (data & (1 << i))
        {
            DS18B20_SetDataPin(sensor, 0);
            DS18B20_DelayUs(sensor, 1);

            DS18B20_SetPinInput(sensor);
            DS18B20_DelayUs(sensor, 60);
        }
        else
        {
            DS18B20_SetDataPin(sensor, 0);
            DS18B20_DelayUs(sensor, 60);

            DS18B20_SetPinInput(sensor);
        }
    }
}

uint8_t DS18B20_ReadData(DS18B20 *sensor)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        DS18B20_SetPinOutput(sensor);
        DS18B20_SetDataPin(sensor, 0);
        DS18B20_DelayUs(sensor, 2);

        DS18B20_SetPinInput(sensor);
        if (DS18B20_ReadDataPin(sensor))
        {
            value |= (1 << i);
        }

        DS18B20_DelayUs(sensor, 60);
    }

    return value;
}

float DS18B20_ReadTempCelsius(DS18B20 *sensor)
{
    DS18B20_StartSensor(sensor);
    DS18B20_WriteData(sensor, 0xCC); // Skip ROM
    DS18B20_WriteData(sensor, 0x44); // Start temperature conversion
    HAL_Delay(800);                  // Aguarda a conversão (800ms)

    DS18B20_StartSensor(sensor);
    DS18B20_WriteData(sensor, 0xCC); // Skip ROM
    DS18B20_WriteData(sensor, 0xBE); // Read Scratchpad

    uint8_t temp1 = DS18B20_ReadData(sensor);
    uint8_t temp2 = DS18B20_ReadData(sensor);

    uint16_t temp_com = (temp2 << 8) | temp1;

    return (float)temp_com / 16.0; // Converte para float antes da divisão
}

float DS18B20_ReadTempFahrenheit(DS18B20 *sensor)
{
    return DS18B20_ReadTempCelsius(sensor) * 1.8 + 32.0;
}