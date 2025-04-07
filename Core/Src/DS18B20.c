#include "DS18B20.h"

static void delay_us(DS18B20 *sensor, uint16_t us)
{
    sensor->htim->Instance->CNT = 0;
    while (sensor->htim->Instance->CNT < us);
}

static void set_data_pin(DS18B20 *sensor, GPIO_PinState state)
{
    HAL_GPIO_WritePin(sensor->port, sensor->pin, state);
}

static void set_pin_output(DS18B20 *sensor)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = sensor->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(sensor->port, &GPIO_InitStruct);
}

static void set_pin_input(DS18B20 *sensor)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = sensor->pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(sensor->port, &GPIO_InitStruct);
}

static GPIO_PinState read_data_pin(DS18B20 *sensor)
{
    return HAL_GPIO_ReadPin(sensor->port, sensor->pin);
}

static void start_sensor(DS18B20 *sensor)
{
    set_pin_output(sensor);
    set_data_pin(sensor, GPIO_PIN_RESET);
    delay_us(sensor, 480);

    set_pin_input(sensor);
    delay_us(sensor, 80);
    read_data_pin(sensor); // Presente, mas o valor não é usado
    delay_us(sensor, 400);
}

static void write_byte(DS18B20 *sensor, uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        set_pin_output(sensor);
        set_data_pin(sensor, GPIO_PIN_RESET);
        if (data & (1 << i))
        {
            delay_us(sensor, 1);
            set_pin_input(sensor);
            delay_us(sensor, 60);
        }
        else
        {
            delay_us(sensor, 60);
            set_pin_input(sensor);
        }
    }
}

static uint8_t read_byte(DS18B20 *sensor)
{
    uint8_t value = 0;

    for (uint8_t i = 0; i < 8; i++)
    {
        set_pin_output(sensor);
        set_data_pin(sensor, GPIO_PIN_RESET);
        delay_us(sensor, 2);
        set_pin_input(sensor);

        if (read_data_pin(sensor) == GPIO_PIN_SET)
        {
            value |= (1 << i);
        }

        delay_us(sensor, 60);
    }

    return value;
}

void DS18B20_Init(DS18B20 *sensor, TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin)
{
    sensor->htim = htim;
    sensor->port = port;
    sensor->pin = pin;
    HAL_TIM_Base_Start(htim);
}

float DS18B20_ReadTempCelsius(DS18B20 *sensor)
{
    start_sensor(sensor);
    HAL_Delay(1);
    write_byte(sensor, 0xCC);  // Skip ROM
    write_byte(sensor, 0x44);  // Start conversion

    HAL_Delay(800);

    start_sensor(sensor);
    write_byte(sensor, 0xCC);  // Skip ROM
    write_byte(sensor, 0xBE);  // Read scratchpad

    uint8_t temp1 = read_byte(sensor);
    uint8_t temp2 = read_byte(sensor);

    int16_t raw_temp = (temp2 << 8) | temp1;

    return (float)raw_temp / 16.0;
}

float DS18B20_ReadTempFahrenheit(DS18B20 *sensor)
{
    return DS18B20_ReadTempCelsius(sensor) * 1.8f + 32.0f;
}

