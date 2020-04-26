# stm32f103-midi
MIDI keyboard made from a toy

The goal of this project is to convert a toy piano from a local thrift store into a MIDI controller. 

I disassembled a toy piano and realized that the 37 button keyboard is a "matrix" of 5x8. There are some resistors in there too, but I am not using them.

So, I had 2 things to implement: USB MIDI interface and a code that will read the keypad and convert the key pressed and releases into notes.

Unfortunately, the toy keyboard doesn't have hardware to identify MIDI velocity, so it is not present.

Current issue: doesn't work well sometimes if more than 3 buttons are pressed at the same time. To investigate.
