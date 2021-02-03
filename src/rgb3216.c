#include "rgb3216.h"

#include "chanfiles/ff.h"
#include "chanfiles/diskio.h"


#define nPlanes 4 ///< Bit depth per R,G,B (4 = (2^4)^3 = 4096 colors)
#define WIDTH 32
#define HEIGHT 16
#define MAXROWS 8

#define ARM_MATH_CM4
#define __FPU_PRESENT 1

#define FFTLEN 2048


#include "Include/arm_math.h"
#include "audio_samples.h"

uint8_t *matrixbuff;
volatile uint8_t plane, row, *buffptr;
arm_rfft_fast_instance_f32 rs1;
int16_t readdata[FFTLEN];
float wavedata[FFTLEN];
float fftout[FFTLEN];
int fullfftfreqarr[FFTLEN/2], freqval=0, offset=0;
bool pt1=1, pt2, pt3, pt4, pt5, readflag;
uint16_t samplefftidx[32], finalledval[32];
FIL file_h;


/*ASstm32 - Delay functions */

void ns33delay(int x)
{
    int j;
    const int z=x;   

    for (j = 0; j < z ; j++)    
        __asm__("nop");  
}

void msdelay(int x)
{
    int j;
    const int z=30000*x;    //technically 45000

    for (j = 0; j < z ; j++)    
        __asm__("nop");
    
}

void usdelay(int x)
{
    int j;
    const int z=30*x;

    for (j = 0; j < z ; j++)    
        __asm__("nop");
    
}

/*ASstm32 - Print numbers via USART */

void usart_print_num2(unsigned int num)
{
    int cnt=0, numc=num;
    if(num==0)
        cnt=1;
    else
    {
        while(numc>0)
        {
            cnt++;
            numc/=10;
        }
    }
    char *arr;
    arr=(char *)malloc(sizeof(char)* (cnt+1) );
    *(arr + cnt)='\0';
    while(cnt>0)
    {
        *(arr + cnt-1)=48+num%10;
        num/=10;
        cnt--;
    }
    usart_print_text2(arr);
}



/*ASstm32 - Print text via USART */

void usart_print_text2(char *arr)
{
    int len=strlen(arr);
    int c=0;
    char ch;
    while(c<len)
    {
        ch=arr[c];
        usart_send_blocking(USART2, ch);
        c++;
    }
    free(arr);
    usart_send_blocking(USART2, '\t');
}

void usart_print_text(char *arr)
{
    
    int len=strlen(arr);
    int c=0;
    char ch;
    while(c<len)
    {
        ch=arr[c];
        usart_send_blocking(USART2, ch);
        c++;
    }
    usart_send_blocking(USART2, '\t');
    
}

/*ASstm32 - Setup the USART for debugging purposes */

void usart_setup(void)
{

    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);


    usart_enable(USART2);
}


/*ASstm32 - Write the value of the colour to the pins C0 - C5 */

void write_data(uint8_t val)
{

    if(val&4)
    MMIO32(0x40020818U) = 1;
    else
    MMIO32(0x40020818U) = 1<<16;

    if(val&8)
    MMIO32(0x40020818U) = 2;
    else
    MMIO32(0x40020818U) = 2<<16;

    if(val&16)
    MMIO32(0x40020818U) = 4;
    else
    MMIO32(0x40020818U) = 4<<16;

    if(val&32)
    MMIO32(0x40020818U) = 8;
    else
    MMIO32(0x40020818U) = 8<<16;

    if(val&64)
    MMIO32(0x40020818U) = 16;
    else
    MMIO32(0x40020818U) = 16<<16;

    if(val&128)
    MMIO32(0x40020818U) = 32;
    else
    MMIO32(0x40020818U) = 32<<16;

}



/*ASstm32 -  Setup the GPIO pins used by the LED matrix panel, the SD Card reader
and USART */

void gpio_setup(void)
{
    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_180MHZ]);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_USART2);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);  //Default inbuilt led
    

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);      //USART
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2);                              //USART

    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);  //C0
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1);  //C1
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);  //C2

    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO3);  //C3
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4);  //C4
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);  //C5

    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO6);  //C6
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);  //C7
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);  //C8

    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);  //C6
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);  //C7
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11);  //C8
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);  //C6
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO13);  //C7
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO14);  //C8
    gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO15);  //C6

    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO1); //B1 - CLK
    //gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO1);

    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2); //B2 - LAT/STB

    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12); //B12 - For Timer 4/OE
}



/*ASstm32 - Enable SPI2 to perform read/write operations on the microSD card */

static void sd_card_setup(void)
{

    //Pin sck -   B13
    //Pin miso -  B14
    //Pin mosi -  B15
    //Pin CS -  A10

    rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN);
    rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI2EN);

    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN,
            GPIO13 | GPIO14 | GPIO15);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO13 | GPIO14 | GPIO15);
    gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ,
                GPIO13 | GPIO15);

    /* Chip select line */
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO10);
    //CS_OFF_SET();
    

    rcc_periph_clock_enable(RCC_SPI2);
    spi_reset(SPI2);


    spi_set_bidirectional_mode(SPI2);
    spi_set_full_duplex_mode(SPI2);
    spi_enable_software_slave_management(SPI2);
    spi_set_nss_high(SPI2);

    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
            SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
    spi_enable_rx_dma(SPI2);
    spi_enable_tx_dma(SPI2);
    spi_enable(SPI2);
    CS_ON_CLR();


}


/*ASstm32 - Enable the DMA channel associated with SPI2 
for enabling DMA transfers from the microSD card*/

static void dma_init(void)
{
    
    CS_OFF_SET();
    //rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_DMA1EN);
    rcc_periph_clock_enable(RCC_DMA1);
    
    dma_stream_reset(DMA1, DMA_STREAM3);
    dma_disable_stream(DMA1,DMA_STREAM3);

    
    dma_set_transfer_mode(DMA1, DMA_STREAM3,DMA_SxCR_DIR_PERIPHERAL_TO_MEM);
    dma_disable_peripheral_increment_mode(DMA1,DMA_STREAM3);
    dma_set_priority(DMA1,DMA_STREAM3,DMA_SxCR_PL_VERY_HIGH);
    dma_enable_transfer_complete_interrupt(DMA1,DMA_STREAM3);
    dma_enable_memory_increment_mode(DMA1,DMA_STREAM3);
    dma_enable_direct_mode(DMA1,DMA_STREAM3);
    //dma_enable_circular_mode(DMA1,DMA_STREAM3);
    //dma_enable_fifo_mode(DMA1,DMA_STREAM3);
    dma_enable_transfer_error_interrupt(DMA1,DMA_STREAM3);
    dma_set_peripheral_size(DMA1,DMA_STREAM3,DMA_SxCR_PSIZE_8BIT);
    dma_set_memory_size(DMA1,DMA_STREAM3,DMA_SxCR_MSIZE_16BIT); //8bit?
    dma_set_peripheral_address(DMA1,DMA_STREAM3,(DWORD) &SPI_DR(SPI2));
    dma_channel_select(DMA1, DMA_STREAM3, DMA_SxCR_CHSEL_0);
    nvic_set_priority(NVIC_DMA1_STREAM3_IRQ, 0);
    nvic_enable_irq(NVIC_DMA1_STREAM3_IRQ);
    CS_ON_CLR();
    
}



/*ASstm32 - Setup the timer to control the RGB LED matrix panel */

void tim4_setup(void)
{
    rcc_periph_clock_enable(RCC_TIM4);
    nvic_enable_irq(NVIC_TIM4_IRQ);
    rcc_periph_reset_pulse(RST_TIM4);
    timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); //Check loc3 example
    timer_set_prescaler(TIM4, 900-1);
    timer_set_period(TIM4, 4622-1);
    //timer_set_period(TIM4, 9244-1);
    #if 1
    timer_disable_oc_output(TIM4, TIM_OC1);
    timer_disable_oc_clear(TIM4, TIM_OC1);
    timer_enable_oc_preload(TIM4, TIM_OC1);
    timer_set_oc_slow_mode(TIM4, TIM_OC1);
    timer_set_oc_mode(TIM4, TIM_OC1, TIM_OCM_PWM1);
    timer_set_oc_polarity_high(TIM4, TIM_OC1);
    timer_set_oc_value(TIM4, TIM_OC1, 0);
    timer_enable_oc_output(TIM4, TIM_OC1);
    #endif
    timer_enable_preload(TIM4);
    timer_enable_counter(TIM4);
    nvic_set_priority(NVIC_TIM4_IRQ, 16);
    timer_enable_irq(TIM4, TIM_DIER_CC1IE);   //prev - TIM_DIER_UDE, timer isr doesnt work
}




/*ASstm32 - The interrupt service routine for Timer 4. 
The ledmaton() function is invoked repeatedly to refresh the LED display
The FFT functions are invoked at certain preset times in order to force 
persistence of vision in the LED panel; not doing so would result in a heavy
flickering.  */

#if 1
void tim4_isr(void)
{

    ledmaton();

    #if 1

    if(timer_get_counter(TIM4)<1000 && pt1)
        fft_func_pt1();
    else if(timer_get_counter(TIM4)>1000 && pt2)
        fft_func_pt2();
    else if(timer_get_counter(TIM4)>2000 && pt3)
        fft_func_pt3();
    else if(timer_get_counter(TIM4)>3000 && pt4)
        fft_func_pt4();
    else if(timer_get_counter(TIM4)>4000 && pt5)
        fft_func_pt5(); 
    #endif


    #if 0

    if(timer_get_counter(TIM4)<2000 && pt1)
        fft_func_pt1();
    else if(timer_get_counter(TIM4)>4000 && pt2)
        fft_func_pt2();
    else if(timer_get_counter(TIM4)>6000 && pt3)
        fft_func_pt3();
    else if(timer_get_counter(TIM4)>7000 && pt4)
        fft_func_pt4();
    else if(timer_get_counter(TIM4)>8900 && pt5)
        fft_func_pt5(); 
    #endif

}
#endif


/*ASstm32 - Function for selecting the row number to write the data on, and then
writing the colour data based on one of the 4 planes */

void ledmaton(void)
{
    uint8_t i, *ptr;

        //gpio_set(GPIOB, GPIO12);
         //gpio_set(GPIOB, GPIO2);
        MMIO32(0x40020418U) = 32768;
        MMIO32(0x40020418U) = 4;


        if (++plane >= nPlanes)
        {
            plane = 0;
            if (++row >= MAXROWS)
            {
                row = 0;
                buffptr = matrixbuff;
            }
        }
        else if (plane == 1)
        {
            if (row & 0x1)
                gpio_set(GPIOC, GPIO6);
            else
                gpio_clear(GPIOC, GPIO6);
            if (row & 0x2)
                gpio_set(GPIOC, GPIO7);
            else
                gpio_clear(GPIOC, GPIO7);
            if (row & 0x4)
                gpio_set(GPIOC, GPIO8);
            else
                gpio_clear(GPIOC, GPIO8);
        }


        ptr = (uint8_t *)buffptr;

        MMIO32(0x40020418U) = (unsigned int)2147483648;
        MMIO32(0x40020418U) = (unsigned int)262144;
        //gpio_clear(GPIOB, GPIO12);
        //gpio_clear(GPIOB, GPIO2);

        if (plane>0)
        {
            //usart_print_text("loop1");
            for(i=0;i<WIDTH;i++)
            {
                write_data(*ptr);
                MMIO32(0x40020418U) = 2;
                gpio_clear(GPIOB, GPIO1);
                //gpio_set(GPIOB, GPIO1);
                //MMIO32((GPIO_PORT_B_BASE) + 0x18) = 131072;
                ptr++;
            }
            buffptr = ptr; //+= 32;
        }
        else
        {
            //usart_print_text("loop2");
            for (i = 0; i < WIDTH; i++)
            {
                uint8_t data2 = (ptr[i] << 6) | ((ptr[i + WIDTH] << 4) & 0x30) |
                ((ptr[i + WIDTH * 2] << 2) & 0x0C);
                write_data(data2);
                MMIO32(0x40020418U) = 2;
                //MMIO32((GPIO_PORT_B_BASE) + 0x18) = 131072;
                //gpio_set(GPIOB, GPIO1);
                gpio_clear(GPIOB, 2);
            }
        }
        
}

/*ASstm32 - The interrupt service routine function for DMA1, for transferring data
from the microSD card using SPI2 */


void dma1_stream3_isr()
{
    while(!dma_get_interrupt_flag(DMA1,DMA_STREAM3,DMA_TCIF ));
    //usart_print_text("Transfer complete");
    dma_clear_interrupt_flags(DMA1, DMA_STREAM3, DMA_TCIF);
    CS_OFF_SET();
}


/*ASstm32 - Initialization of the RGB LED matrix panel*/


void matrix_init(void)
{
    uint16_t maxsize = WIDTH*MAXROWS*3;
    matrixbuff=(uint8_t*)malloc(sizeof(uint8_t)*maxsize);
    memset(matrixbuff, 0, maxsize);

     *((volatile uint32_t*) 0x40023C00) |= (0b111<<8);

    plane = 0;
    row = MAXROWS - 1;

    gpio_clear(GPIOB, GPIO1);
    gpio_clear(GPIOB, GPIO2);
    gpio_set(GPIOB, GPIO15);

    gpio_clear(GPIOC, GPIO6);
    gpio_clear(GPIOC, GPIO7);
    gpio_clear(GPIOC, GPIO8);

    gpio_clear(GPIOC, GPIO0);
    gpio_clear(GPIOC, GPIO1);
    gpio_clear(GPIOC, GPIO2);
    gpio_clear(GPIOC, GPIO3);
    gpio_clear(GPIOC, GPIO4);
    gpio_clear(GPIOC, GPIO5);

    write_data(0);
}



/*ASstm32 - For lighting up one LED in the matrix panel, based on the x and y 
coordinates. The colour is split in the RGB components and bits are enabled in
the matrixbuff based on the colours, planes and the row group (upper 8 or lower 8) */


void drawpixel(uint8_t x, uint8_t y, uint16_t colour)
{
    uint8_t r, g, b, bit, limit, *ptr;

    if (x >= WIDTH || y >= HEIGHT)
    return;

    r = colour >> 8;
    g = (colour >> 4) & 0xF;
    b = (colour) & 0xF;

    bit = 2;
    limit = 1 << nPlanes;

    if (y < MAXROWS)
    {
        ptr = &matrixbuff[y * WIDTH * (nPlanes - 1) + x]; // Base addr
        ptr[WIDTH * 2] &= 0b11111100;

        if (r & 1)
          ptr[WIDTH * 2] |= 0b00000001;
        if (g & 1)
          ptr[WIDTH * 2] |= 0b00000010;
        if (b & 1)
          ptr[WIDTH] |= 0b00000001;
        else
          ptr[WIDTH] &= 0b11111110;

        for (; bit < limit; bit <<= 1) 
        {
            *ptr &= ~0b00011100; 
                if (r & bit)
            *ptr |= 0b00000100;
                if (g & bit)
            *ptr |= 0b00001000;
                if (b & bit)
            *ptr |= 0b00010000;
            ptr += WIDTH; 
        }
  }
    else
    {
        ptr = &matrixbuff[(y - MAXROWS) * WIDTH * (nPlanes - 1) + x];
        *ptr &= ~0b00000011;

        if (r & 1)
          ptr[WIDTH] |= 0b00000010;
        else
          ptr[WIDTH] &= ~0b00000010;

        if (g & 1)
          *ptr |= 0b00000001;

        if (b & 1)
          *ptr |= 0b00000010;

        for (; bit < limit; bit <<= 1)
        {
            *ptr &= ~0b11100000;
            if (r & bit)
                *ptr |= 0b00100000;
            if (g & bit)
                *ptr |= 0b01000000;
            if (b & bit)
                *ptr |= 0b10000000; 
            ptr += WIDTH;        
        }
    }
}



/*ASstm32 - Calulate the magnitude*/

float power(float real, float compl) 
{
    return (real*real+compl*compl);
}

/*ASstm32 - Convert the frequncies in the frequency list samplefftidx to 
equivalent indices that can be used on the FFT output later on*/

void fft_func_pt0(void)
{
    for(int i=0;i<32;i++)
        samplefftidx[i] = ceil(((float)(samplefreq[i]*(FFTLEN/2) ))/22050.00000);
}

/*ASstm32 - Read FFTLEN*2 bytes from the Wave audio file, converted into 
FFTLEN signed 16-bit integers */

void fft_func_pt1(void)
{
    pt1=0;
    f_read(&file_h, readdata, FFTLEN*2, NULL);
    pt2=1;
}

/*ASstm32 - Normalize the data read from the audio file and apply a hamming window
for more accurate FFT results */

void fft_func_pt2(void)
{
    pt2=0;
    #if 1
    for(int j=0;j<FFTLEN;j++)
    {
        wavedata[j] = (float)( readdata[j] / 32768.0000000);
        wavedata[j]*=windowdata[j];
    }
    #endif
    pt3=1;
}


/*ASstm32 - Perform the FFT on the audio data using Cortex M4F's 
inbuilt DSP capabilities */

void fft_func_pt3(void)
{
    pt3=0;
    arm_rfft_fast_f32(&rs1, wavedata, fftout, 0);
    pt4=1;
}


/*ASstm32 - Map the FFT output to the 32 bands we're using in the LED matrix */

void fft_func_pt4(void)
{
    pt4=0;
    for (int i=0; i<FFTLEN; i=i+2) 
    {
        fullfftfreqarr[freqval] = (int)power(fftout[i], fftout[i+1]);
        if (fullfftfreqarr[freqval]<0) 
            fullfftfreqarr[freqval]=0;
        freqval++;
    }

    freqval=0;

    for(int i=0;i<31;i++)
    {
        for (int j=samplefftidx[i]; j<samplefftidx[i+1]; j++) 
        {
            finalledval[i]+=fullfftfreqarr[j];
        }
        finalledval[i]=(20*log10f(sqrtf(finalledval[i]))) /3;
    }
    
    pt5=1;
    
}


/*ASstm32 - Call drawpixel() for the columns that need to be lit up */

void fft_func_pt5(void)
{
    pt5=0;
    //usart_print_text("soundlight");
    for(int i=0;i<32;i++)
    {
        for(int j=15;j>(15-finalledval[i]);j--)
            drawpixel(i,j,ledcolor[j]);

        for(int j=0;j<=(15-finalledval[i]);j++)
            drawpixel(i,j,0x000);
    }

    pt1=1;
}


/*ASstm32 - Function to light up one column of LEDs*/

void soundlighttest(void)
{
    drawpixel(10,15,0x030);
    drawpixel(10,14,0x030);
    drawpixel(10,13,0x030);
    drawpixel(10,12,0x030);
    drawpixel(10,11,0x030);

    drawpixel(10,10,0xF40);
    drawpixel(10,9,0xF40);
    drawpixel(10,8,0xF40);
    drawpixel(10,7,0xF40);
    drawpixel(10,6,0xF40);
    drawpixel(10,5,0xF40);

    drawpixel(10,4,0xF00);
    drawpixel(10,3,0xF00);
    drawpixel(10,2,0xF00);
    drawpixel(10,1,0xF00);
    drawpixel(10,0,0xF00);
}


int main(void)
{
    gpio_setup();
    usart_setup();
    matrix_init();
    sd_card_setup();
    dma_init();
    arm_rfft_fast_init_f32(&rs1, FFTLEN);
    fft_func_pt0();
    for(int i=0;i<32;i++)
        usart_print_num2(samplefftidx[i]);
    //soundlighttest();
    msdelay(100);

    FATFS fstest;      
     
    FRESULT fileStatus;
    char fsid[2];
    fsid[0] = '0';
    fsid[1] = '\0';
    
    f_mount(&fstest, fsid, 0);

    fileStatus=f_open(&file_h, "spine.wav", FA_OPEN_EXISTING | FA_READ);
    if(fileStatus==FR_OK)
        usart_print_text("file opened successfully");
    else
        usart_print_text("file open failed");

    f_lseek(&file_h, 44);
    tim4_setup();

    
    while (1) 
    {
    }
    return 0;
}