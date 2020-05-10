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
					midiMessage(MIDI_NOTE_ON, 0, firstNote + note, 100);
				}
			}
		}

		previousKeypadState = keypadState;
	}
}

uint8_t GetRowState(GPIO_TypeDef* gpio, uint16_t pin)
{
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);

	uint32_t oldResult = 0;
	uint32_t result = 0;
	uint8_t sameResultCount = 5;
	uint8_t allResultCount = 10;
	while (sameResultCount > 0 && allResultCount > 0)
	{
		result = GPIOB->IDR & (GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8); // B4,B5,B6,B7,B8
		if (oldResult == result)
		{
			sameResultCount--;
		}
		else
		{
			sameResultCount = 5;
			oldResult = result;
		}

		allResultCount--;
	}

	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);

	return (uint8_t)(result >> 4);
}

uint64_t GetColumnState(GPIO_TypeDef* gpio, uint16_t pin)
{
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_SET);

	uint64_t oldResult = 0;
	uint64_t result = 0;
	uint8_t sameResultCount = 5;
	uint8_t allResultCount = 10;
	uint32_t resultA;
	uint32_t resultB;
	while (sameResultCount > 0 && allResultCount > 0)
	{
		resultA = GPIOA->IDR & (GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10); // A8,A9,A10
		resultB = GPIOB->IDR & (GPIO_PIN_9
				|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_14);  // B9,B12,B13,B14,B15
		result = 0;
		if (resultB & GPIO_PIN_9)
		{
			result |= 1;
		}
		if (resultA & GPIO_PIN_10)
		{
			result |= 1 << 5;
		}
		if (resultA & GPIO_PIN_9)
		{
			result |= 1 << 10;
		}
		if (resultA & GPIO_PIN_8)
		{
			result |= 1 << 15;
		}
		if (resultB & GPIO_PIN_15)
		{
			result |= 1 << 20;
		}
		if (resultB & GPIO_PIN_14)
		{
			result |= 1 << 25;
		}
		if (resultB & GPIO_PIN_13)
		{
			result |= 1 << 30;
		}
		if (resultB & GPIO_PIN_12)
		{
			result |= (uint64_t)1 << 35;
		}

		if (oldResult == result)
		{
			sameResultCount--;
		}
		else
		{
			sameResultCount = 5;
			oldResult = result;
		}

		allResultCount--;
	}

	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);

	return result;
}

uint64_t GetKeypadStateByRow()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Configure GPIO pins : PB9 PB12 PB13 PB14 PB15 */
	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configure GPIO pins : PA8 PA9 PA10 */
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure GPIO pins : PB4 PB5 PB6 PB7 PB8 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	uint64_t result = 0;
	uint8_t rowState;

	rowState = GetRowState(GPIOB, GPIO_PIN_12);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOB, GPIO_PIN_13);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOB, GPIO_PIN_14);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOB, GPIO_PIN_15);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOA, GPIO_PIN_8);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOA, GPIO_PIN_9);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOA, GPIO_PIN_10);
	result |= rowState;
	result <<= 5;

	rowState = GetRowState(GPIOB, GPIO_PIN_9);
	result |= rowState;

	return result;
}

uint64_t GetKeypadStateByColumn()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Configure GPIO pins : PB9 PB12 PB13 PB14 PB15 */
	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configure GPIO pins : PA8 PA9 PA10 */
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure GPIO pins : PB4 PB5 PB6 PB7 PB8 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	uint64_t result = 0;
	uint64_t columnState;

	columnState = GetColumnState(GPIOB, GPIO_PIN_4);
	result |= columnState;

	columnState = GetColumnState(GPIOB, GPIO_PIN_5);
	result |= columnState << 1;

	columnState = GetColumnState(GPIOB, GPIO_PIN_6);
	result |= columnState << 2;

	columnState = GetColumnState(GPIOB, GPIO_PIN_7);
	result |= columnState << 3;

	columnState = GetColumnState(GPIOB, GPIO_PIN_8);
	result |= columnState << 4;

	return result;
}

uint64_t GetKeypadState()
{
	uint64_t resultByRow = GetKeypadStateByRow();
	uint64_t resultByColumn = GetKeypadStateByColumn();

	return resultByRow | resultByColumn;
}
