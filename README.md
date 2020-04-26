# MIDI keyboard made from a toy

The goal of this project is to convert a toy piano from a local thrift store into a MIDI controller. 

I disassembled a toy piano and realized that the 37 button keyboard is a "matrix" of 5x8. There are some resistors in there too, but I am not using them. Here is how I decided to connect the wires to the board:

![Pinout](https://raw.githubusercontent.com/abelykh0/stm32f103-midi/master/doc/Pinout.png)

So, I had 2 things to implement: USB MIDI interface and a code that will read the keypad and convert the key presses and releases into notes.

Unfortunately, the toy keyboard doesn't have hardware to identify MIDI velocity, so it is not present.

Current issue: doesn't work well sometimes if more than 3 buttons are pressed at the same time. To investigate.

## Hardware

| Hardware      |    Qty|
| ------------- | -----:|
| ["blue pill"](https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill) STM32F103C8 board | 1
| Toy piano | 1
| Old IDE connector | 1
| ST-Link v2 or clone | 1

## Software

[System Workbench for STM32](https://www.st.com/en/development-tools/sw4stm32.html/)
