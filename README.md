<h3>Introduction</h3>

This project uses the inbuilt DSP capabilities of the Cortex M4F CPU to calculate the FFT of audio data from an uncompressed/WAVE audio file and illustrate the intensity of different frequency bands in an RGB LED matrix panel.

<h3>Video</h3>

I have uploaded a demo of my project to Youtube, check it out here: https://youtu.be/9FIgD8RN5yE

![output](https://user-images.githubusercontent.com/7463848/106691030-17fea000-65d3-11eb-9f09-0406d67e1159.gif)


<h3>Software Requirements</h3>

1. GCC-ARM toolchain (Preferably installed in /opt/ directory)

2. Libopencm3 Library - Libopencm3 is an low-level, open-source library for STM32 development. The libopencm3 folder in this repository is a heavily trimmed version of the original containing the required files needed to compile code only for the STM32F4 platform. But the latest libopencm3 can be obtained from https://github.com/libopencm3/libopencm3

3. ST-Linkv2 for flashing binary files to the microcontroller (https://freeelectron.ro/installing-st-link-v2-to-flash-stm32-targets-on-linux/)

<h3>Hardware Requirements</h3>

1. An RGB LED panel. The one used in this project can be obtained from Adafruit: https://www.adafruit.com/product/420

2. A microSD card reader. This project uses the one by Sparkfun: https://www.sparkfun.com/products/544

3. An STM32F4 microcontroller. This project uses the NUCLEO-F446RE board.

<h3>Important Files list</h3>

1. src/rgb3216.c - Written almost entirely by me; Contains the intialisation functions for the GPIO pins, the RGB panel and, the microSD card reader. Also contains a timer to control the RGB panel and all the FFT functions needed to calculate the FFT of the audio data.

2. MATLAB - The folder contains a sample MATLAB code to generate a Blackmann-Harris window which can be applied to the audio data before the FFT operation for more accurate results (https://en.wikipedia.org/wiki/Window_function). Folder also contains a sample WAV audio file of frequency 1690Hz and sampling rate 48 Khz to test the MATLAB code with. 

3. chanfiles/diskio.c - Contains the application layer for invoking the FatFs APIs, for reading and writing data from the microSD card. I ported the example code provided by Mbed to libopencm3 ( https://os.mbed.com/cookbook/SD-Card-File-System)

4. chanfiles - The other files in this folder contain the open-source FatFs driver from ChaN (http://elm-chan.org/fsw/ff/00index_e.html)

5. Include - This folder contains the header files of ARM's CMSIS DSP library, taken from here: https://github.com/ARM-software/CMSIS/tree/master/CMSIS/Include


<h3>Building and Compilation</h3>

Run make on the root folder to build the entire project, including the src files and the libopencm3 library

If changes have done to the files in the src folder, run the zbuildsd.sh script to build only the src files

Run the zbuildcopy.sh script to generate bin files and automatically copy them to the STM32F4 microcontroller

<h3>Pin-Mapping</h3>

<table>
<thead>
  <tr>
    <th>MicroSD <br>Reader</th>
    <th>STM32F4</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>GND</td>
    <td>GND</td>
  </tr>
  <tr>
    <td>CO</td>
    <td>NIL</td>
  </tr>
  <tr>
    <td>DO</td>
    <td>B14</td>
  </tr>
  <tr>
    <td>SCK</td>
    <td>B13</td>
  </tr>
  <tr>
    <td>DI</td>
    <td>B15</td>
  </tr>
  <tr>
    <td>CS</td>
    <td>A10</td>
  </tr>
  <tr>
    <td>VCC</td>
    <td>3v3</td>
  </tr>
</tbody>
</table>


<table>
<thead>
  <tr>
    <th>RGB LED <br>Matrix</th>
    <th>STM32F4</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td>R1</td>
    <td>C0</td>
  </tr>
  <tr>
    <td>G1</td>
    <td>C1</td>
  </tr>
  <tr>
    <td>B1</td>
    <td>C2</td>
  </tr>
  <tr>
    <td>R2</td>
    <td>C3</td>
  </tr>
  <tr>
    <td>G2</td>
    <td>C4</td>
  </tr>
  <tr>
    <td>B2</td>
    <td>C5</td>
  </tr>
  <tr>
    <td>A</td>
    <td>C6</td>
  </tr>
  <tr>
    <td>B</td>
    <td>C7</td>
  </tr>
  <tr>
    <td>C</td>
    <td>C8</td>
  </tr>
  <tr>
    <td>CLK</td>
    <td>B1</td>
  </tr>
  <tr>
    <td>STB</td>
    <td>B2</td>
  </tr>
  <tr>
    <td>OE</td>
    <td>B12</td>
  </tr>
  <tr>
    <td>GND</td>
    <td>GND</td>
  </tr>
</tbody>
</table>



<h3>Issues/To Be Implemented</h3>

There's some flickering and ghosting effects in the LED panel that are yet to be fixed.
I'm planning to interface a speaker set using I2S or DAC so that the audio can play while the spectrum analyzer is running (For the demo video above, I edited the audio into the video)
I'm also planning to read audio directly from an ADC interface.

<h3>Downloads</h3>

Custom audio tones of different frequencies, sound level and duration can be generated using this tool: https://www.audiocheck.net/audiofrequencysignalgenerator_sinetone.php

<h3>Acknowledgements</h3>

Since there's no hardware reference manual for this LED matrix, I had to port the code written by Adafruit (for the Arduino platform) to Libopencm3. I only ported the sections that were enough for my project. Adafruit's library is available here: https://github.com/adafruit/RGB-matrix-Panel

This project uses the FatFs - Generic FAT Filesystem Module library from ChaN.
