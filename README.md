<h3>Introduction</h3>

This project uses the inbuilt DSP capabilities of the Cortex M4F CPU to calculate the FFT of audio data from an uncompressed/WAVE audio file and illustrate the intensity of the frequencies in an RGB LED matrix panel.

<h3>Video</h3>

I have uploaded a demo of my project to Youtube, check it out here: 

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/9FIgD8RN5yE/0.jpg)](https://www.youtube.com/watch?v=9FIgD8RN5yE)


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

2. chanfiles/diskio.c - Contains the application layer for invoking the FatFs APIs, for reading and writing data from the microSD card. I ported the example code provided by Mbed to libopencm3 ( https://os.mbed.com/cookbook/SD-Card-File-System)

3. chanfiles - The other files in this folder contain the open-source FatFs driver from ChaN (http://elm-chan.org/fsw/ff/00index_e.html)


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
    <th>RGB LED Matrix</th>
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



<h3>Performance Analysis</h3>

SPI has limited bandwidth of about 0.5 - 1 MBps and the size of the uncompressed video file was about 40 MB and hence the framerate is really slow. (First image above)

After integrating a JPEG decoder to read compressed/MJPEG video files instead, the framerate improved, but is still far from perfect. Since STM32F446RE's SRAM is only 128 KB, it wasn't enough to process a 320x240 MJPEG video, which would have needed 150 KB of SRAM instead. So the video was resized to 240x180. (Second image above)

<h3>Downloads</h3>

The RGB file used for testing: https://drive.google.com/file/d/1naavXjKioFAy7kV9V69ojJpQR5Z7ykuu/view?usp=sharing

The MJPEG file used for testing: https://drive.google.com/file/d/1dNoX6LIpb77QD0pMJv0v4orD502aLdtR/view?usp=sharing

<h3>Acknowledgements</h3>

This project uses the Tiny JPEG Decompressor and the FatFs - Generic FAT Filesystem Module libraries from ChaN.

