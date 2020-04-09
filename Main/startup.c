#include "startup.h"
#include <string.h>
#include <stdint.h>
#include "Sound/midiMessage.h"

static int firstNote = 53;

static uint64_t keypadState = 0;
static uint64_t previousKeypadState = 0;

uint64_t GetKeypadState();

void initialize()
{
}

void setup()
{
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
	uint8_t result = GPIOA->IDR & (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4); // A0,A1,A2,A3,A4
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);

	return result;
}

uint64_t GetColumnState(GPIO_TypeDef* gpio, uint16_t pin)
{
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);
	uint16_t resultA = GPIOA->IDR & (GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);              // A5,A6,A7
	uint16_t resultC = GPIOC->IDR & (GPIO_PIN_13);                                   // C13
	uint16_t resultB = GPIOB->IDR & (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11); // B0,B1,B10,B11
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);

	uint64_t result = 0;

	if (resultA & GPIO_PIN_5)
	{
		result |= 1;
	}
	if (resultA & GPIO_PIN_6)
	{
		result |= 1 << 5;
	}
	if (resultA & GPIO_PIN_7)
	{
		result |= 1 << 10;
	}
	if (resultC & GPIO_PIN_13)
	{
		result |= 1 << 15;
	}
	if (resultB & GPIO_PIN_0)
	{
		result |= 1 << 20;
	}
	if (resultB & GPIO_PIN_1)
	{
		result |= 1 << 25;
	}
	if (resultB & GPIO_PIN_10)
	{
		result |= 1 << 30;
	}
	if (resultB & GPIO_PIN_11)
	{
		result |= (uint64_t)1 << 35;
	}

	return result;
}

uint64_t GetKeypadStateByRow()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA0 PA1 PA2 PA3
						   PA4 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
						  |GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA5 PA6 PA7 */
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB10 PB11 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	uint64_t result = 0;
	uint8_t rowState;

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

uint64_t GetKeypadStateByColumn()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA0 PA1 PA2 PA3
						   PA4 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
						  |GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA5 PA6 PA7 */
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB10 PB11 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	uint64_t result = 0;
	uint64_t columnState;

	columnState = GetColumnState(GPIOA, GPIO_PIN_0);
	result |= columnState;

	columnState = GetColumnState(GPIOA, GPIO_PIN_1);
	result |= columnState << 1;

	columnState = GetColumnState(GPIOA, GPIO_PIN_2);
	result |= columnState << 2;

	columnState = GetColumnState(GPIOA, GPIO_PIN_3);
	result |= columnState << 3;

	columnState = GetColumnState(GPIOA, GPIO_PIN_4);
	result |= columnState << 4;

	return result;
}

uint64_t GetKeypadState()
{
	uint64_t resultByRow = GetKeypadStateByRow();
	uint64_t resultByColumn = GetKeypadStateByColumn();

	return resultByRow | resultByColumn;
}
