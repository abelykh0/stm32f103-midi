#include "startup.h"
#include <string.h>
#include <stdint.h>
#include "Sound/midiMessage.h"

static int firstNote = 53;

static uint64_t keypadState = 0;
static uint64_t previousKeypadState = 0;

uint64_t
GetKeypadState();

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
	//HAL_Delay(1);
	uint8_t result = GPIOA->IDR & 0x1F;
	HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_RESET);
	//HAL_Delay(1);

	return result;
}

uint64_t GetKeypadState()
{
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
