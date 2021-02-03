#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/dma.h>
//#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dac.h>

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CS_ON_CLR()       gpio_clear(GPIOA, GPIO10)    /* MMC CS = L */ //For setting CS on, send low bit
#define CS_OFF_SET()      gpio_set(GPIOA, GPIO10)      /* MMC CS = H */ //For setting CS off, send high bit


void tim4_setup(void);
void gpio_setup(void);
void dma_setup(enum rcc_periph_clken dma_clken, uint32_t dma_controller, 
                uint8_t dma_stream, uint8_t nvic_dma_irq, uint32_t tim_cc_reg, uint16_t *data_block);
void show(void);
void ns33delay(int);
void msdelay(int);
void matrix_init(void);
void drawpixel(uint8_t, uint8_t, uint16_t);
void hex_to_arr(uint8_t);
void send_data(uint8_t[]);
void usdelay(int x);
void usart_print_num2(unsigned int);
void usart_print_text(char *);
void usart_print_text2(char *);
void usart_setup(void);
void matrix_loop(void);
void test_gpio(void);
void gpio_clear2(uint32_t, uint16_t);
void write_data(uint8_t);
float complexABS(float, float);
void fft_func(void);
void ledmaton(void);
void soundlighttest(void);
void soundlighttest2(void);
void soundlight(void);
void fft_func_pt0(void);
void fft_func_pt1(void);
void fft_func_pt2(void);
void fft_func_pt3(void);
void fft_func_pt4(void);
void fft_func_pt5(void);
void fft_func_pt6(void);
void tim3_setup(void);
void readfile(void);
void ledmaton2(void);
float power(float, float);