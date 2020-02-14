## Font tools for TFT library

---

This directory contains programs to convert TrueType fonts (ttf) into other formats suitable
for use on the ESP32.  

The first program can convert a .ttf font file into a C source file. The C source files can be
included in the ESP32 tft library during compilation.

The second program can convert a font in a C source file into a .fon file. 
These .fon files can be stored in a SPIFFS image and loaded by a program running on the ESP32.


---

### Converting TTF to C Source Files

The program to convert from TTF to C is called `ttf2c_vc2003.exe`.  This is a windows program, but can be used under Linux with wine:

**Usage:**

`ttf2c_vc2003.exe <point-size> <input-file> <output-file> [<start_char> <end_char>]`

or, under Linux:

`wine ./ttf2c_vc2003.exe <point-size> <input-file> <output-file> [<start_char> <end_char>]`


#### Example:

`wine ./ttf2c_vc2003.exe 18 Vera.ttf vera.c`

**IMPORTANT:**  After the c source is created, open it in editor and make following changes:

Delete the line:  
`#include <avr/pgmspace.h>`

Change the line:  
`uint8_t vera18[] PROGMEM =`

to:  
`unsigned char vera18[] =`

The font name above (i.e. `vera18`) will be different for different fonts, 
you can change it to any other name.


---

### Converting C Sources Files to .fon Files

The program to convert C source files to .fon files is call `c2fon`. 
This program must be compiled prior to use.  

To compile c2fon:  
`make`

After compiling you can run c2fon on C source files to generate .fon files. 

**Usage:**

`c2fon <C-formatted font file> <.fon output file>`


#### Example:

`c2fon ./c_font_files/vera.c  ./compiled_font_files/vera.fon`
