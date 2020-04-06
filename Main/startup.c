#include "startup.h"
#include <string.h>
#include <stdint.h>
#include "Sound/midiMessage.h"

extern ADC_HandleTypeDef hadc1;

static int firstNote = 53;

#define KEYPAD_ROWS 5
static uint64_t keypadState = 0;
static uint64_t previousKeypadState = 0;
static volatile uint8_t adcReady;
static volatile uint16_t adc[KEYPAD_ROWS];

uint64_t GetKeypadState();

void initialize()
{
}

void setup()
{
	HAL_ADCEx_Calibration_Start(&hadc1);
}

void loop()
{
	keypadState = GetKeypadState();
	if (keypadState != previousKeypadState)
	{
		for (int note = 0; note < 37; note++)
		{
			uint64_t noteBit = (uint64_t) 1 << note;
			if ((keypadState & noteBit) != (previousKeypadState & noteBit))
			{
				if ((keypadState & noteBit) == 0)
				{
					// note released
					midiMessage(MIDI_NOTE_OFF, 0, firstNote + note, 0);
				}
				else
				{
					// note pressed
					midiMessage(MIDI_NOTE_ON, 0, firstNote + note, 0x70);
				}
			}
		}

		previousKeypadState = keypadState;
	}
}

uint8_t GetRowState(GPIO_TypeDef* gpio, uint16_t pin)
{
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);

	// Using ADC because this keyboard have resistors
	// for some reason
	adcReady = 0;
	HAL_ADC_Start(&hadc1);
	HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc, KEYPAD_ROWS);
    while (!adcReady)
    {
    	// wait
    }

	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);
	HAL_ADC_Stop(&hadc1);

	uint8_t result = 0;
    for (int i = 0; i < KEYPAD_ROWS; i++)
    {
    	if (i > 0)
    	{
    		result <<= 1;
    	}

    	if (adc[i] > 4000)
    	{
    		result |= 1;
    	}
    }

	return result;
}

uint64_t GetKeypadState()
{
	uint64_t result = 0;
	uint8_t rowState;

	//HAL_Delay(1);

	rowState = GetRowState(GPIOB, GPIO_PIN_11);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOB, GPIO_PIN_10);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOB, GPIO_PIN_1);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOB, GPIO_PIN_0);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOC, GPIO_PIN_13);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOA, GPIO_PIN_7);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOA, GPIO_PIN_6);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOA, GPIO_PIN_5);
	result |= rowState;

	return result;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        HAL_ADC_Stop_DMA(&hadc1);
        //snprintf(trans_str, 63, "ADC %d %d\n", (uint16_t)adc[0], (uint16_t)adc[1]);
        //HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 1000);
        adcReady = 1;
    }
}
