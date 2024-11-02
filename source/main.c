#include "main.h"
#include <stm32h7xx_hal.h>
#include <stm32h7xx_ll_rcc.h>
#include "system_stm32h7xx.h"
#include "miniz.h"
#include "libalarmo/lcd.h"
#include "libalarmo/input.h"

#include "doom1_wad.h"

static void DMA_Init(void);
static void FMC_Init(void);
static void TIM3_Init(void);
static void MDMA_Init(void);
static void ADC_Init(void);

extern void D_DoomMain(void);

SRAM_HandleTypeDef fmcHandle;
TIM_HandleTypeDef tim3Handle;
MDMA_HandleTypeDef mdmaHandle;
ADC_HandleTypeDef adcHandle;
DMA_HandleTypeDef dmaHandle;
ADC_HandleTypeDef adc2Handle;
DMA_HandleTypeDef dma2Handle;

uint8_t doom1_wad_unc[doom1_wad_unc_len];
uint32_t doom1_wad_unc_size = doom1_wad_unc_len;

int main(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
    __enable_irq();

    HAL_Init();

    // TODO we just run with whatever the 2ndloader configured for now
    // SystemClock_Config();
    // PeriphClock_Config();

    // Enable DMA streams
    DMA_Init();

    // Initialize FMC for interfacing with the LCD
    FMC_Init();

    // Initialize MDMA for DMA LCD transfers
    MDMA_Init();

    // Initialize timer 3 for the LCD backlight
    TIM3_Init();

    // Enable the ADCs for the dial wheel
    ADC_Init();

    // Initialize inputs
    INPUT_Init();

    // Initialize the LCD
    LCD_Init();

    uint32_t lcdId = 0;
    LCD_RDID(&lcdId);

    // Setup backlight
    LCD_SetBrightness(1.0f);

    // Clear screen
    LCD_DrawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);

    // Turn on the display
    LCD_DISPON();

    // Uncompress the wad
    uncompress(doom1_wad_unc, &doom1_wad_unc_size, doom1_wad, doom1_wad_len);

    // Run doom
    D_DoomMain();

    while (1)
        ;
}

uint32_t get_systime(void)
{
    return HAL_GetTick();
}

void sleep_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

static void DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}

static void FMC_Init(void)
{
    FMC_NORSRAM_TimingTypeDef timing;

    fmcHandle.Instance = FMC_Bank1_R;
    fmcHandle.Extended = FMC_Bank1E_R;

    fmcHandle.Init.WaitSignalActive   = 0;
    fmcHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
    fmcHandle.Init.NSBank             = FMC_NORSRAM_BANK1;
    fmcHandle.Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_16;
    fmcHandle.Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;
    fmcHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
    fmcHandle.Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;
    fmcHandle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;

    timing.BusTurnAroundDuration = 0;
    timing.CLKDivision           = 1;
    timing.DataLatency           = 0;
    timing.AccessMode            = 0;
    timing.DataSetupTime         = 2;
    timing.AddressSetupTime      = 2;   
    timing.AddressHoldTime       = 0;
    if (HAL_SRAM_Init(&fmcHandle, &timing, NULL) != HAL_OK) {
        while (1)
            ;
    }

    HAL_SetFMCMemorySwappingConfig(FMC_SWAPBMAP_SDRAM_SRAM);
}

static void TIMx_PWM_MspInit(TIM_HandleTypeDef *handle)
{
    GPIO_InitTypeDef gpioConfig = { 0 };

    if (handle->Instance == TIM3) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        gpioConfig.Alternate = GPIO_AF2_TIM3;
        gpioConfig.Pull = GPIO_NOPULL;
        gpioConfig.Speed = GPIO_SPEED_FREQ_LOW;
        gpioConfig.Pin = GPIO_PIN_1;
        gpioConfig.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(GPIOB, &gpioConfig);

        gpioConfig.Pin = GPIO_PIN_8;
        gpioConfig.Alternate = GPIO_AF2_TIM3;
        gpioConfig.Speed = GPIO_SPEED_FREQ_LOW;
        gpioConfig.Pull = GPIO_NOPULL;
        gpioConfig.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(GPIOC, &gpioConfig);
    }
}

static void TIM3_Init(void)
{
    TIM_MasterConfigTypeDef masterConfig = { 0 };
    TIM_OC_InitTypeDef channelConfig = { 0 };

    tim3Handle.Instance = TIM3;
    tim3Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    tim3Handle.Init.Prescaler         = 0;
    tim3Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    tim3Handle.Init.Period            = 0xffff;
    tim3Handle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_PWM_Init(&tim3Handle) != HAL_OK) {
        while (1)
            ;
    }

    masterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    masterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    if (HAL_TIMEx_MasterConfigSynchronization(&tim3Handle, &masterConfig) != HAL_OK) {
        while (1)
            ;
    }

    channelConfig.OCFastMode = TIM_OCFAST_DISABLE;
    channelConfig.Pulse      = 0;
    channelConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    channelConfig.OCMode     = TIM_OCMODE_PWM1;
    if (HAL_TIM_PWM_ConfigChannel(&tim3Handle, &channelConfig, TIM_CHANNEL_3) != HAL_OK) {
        while (1)
            ;
    }

    if (HAL_TIM_PWM_ConfigChannel(&tim3Handle, &channelConfig, TIM_CHANNEL_4) != HAL_OK) {
        while (1)
            ;
    }

    TIMx_PWM_MspInit(&tim3Handle);
}

static void MDMA_Init(void)
{
    __HAL_RCC_MDMA_CLK_ENABLE();

    mdmaHandle.Instance = MDMA_Channel0;

    mdmaHandle.Init.BufferTransferLength     = 0x80;
    mdmaHandle.Init.DataAlignment            = MDMA_DATAALIGN_PACKENABLE;
    mdmaHandle.Init.Request                  = MDMA_REQUEST_SW;
    mdmaHandle.Init.DestinationInc           = MDMA_DEST_INC_DISABLE;
    mdmaHandle.Init.SourceDataSize           = MDMA_SRC_DATASIZE_HALFWORD;
    mdmaHandle.Init.DestDataSize             = MDMA_DEST_DATASIZE_HALFWORD;
    mdmaHandle.Init.TransferTriggerMode      = MDMA_BLOCK_TRANSFER;
    mdmaHandle.Init.Priority                 = MDMA_PRIORITY_VERY_HIGH;
    mdmaHandle.Init.Endianness               = MDMA_LITTLE_BYTE_ENDIANNESS_EXCHANGE;
    mdmaHandle.Init.SourceInc                = MDMA_SRC_INC_HALFWORD;
    mdmaHandle.Init.SourceBurst              = MDMA_SOURCE_BURST_SINGLE;
    mdmaHandle.Init.DestBurst                = MDMA_DEST_BURST_SINGLE;
    mdmaHandle.Init.SourceBlockAddressOffset = 0x0;
    mdmaHandle.Init.DestBlockAddressOffset   = 0x0;

    if (HAL_MDMA_Init(&mdmaHandle) != HAL_OK) {
        while (1)
            ;
    }

    // TODO
    // HAL_NVIC_SetPriority(MDMA_IRQn, 5, 0);
    // HAL_NVIC_EnableIRQ(MDMA_IRQn);
}

static void ADC_Init(void)
{
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = { 0 };
    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    RCC_PeriphCLKInitStruct.AdcClockSelection    = RCC_ADCCLKSOURCE_PLL2;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

    adcHandle.Instance = ADC1;

    adcHandle.Init.Overrun                            = ADC_OVR_DATA_OVERWRITTEN;
    adcHandle.Init.LeftBitShift                       = ADC_LEFTBITSHIFT_NONE;
    adcHandle.Init.OversamplingMode                   = ENABLE;
    adcHandle.Init.Oversampling.Ratio                 = 0x10;
    adcHandle.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_4;
    adcHandle.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
    adcHandle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
    adcHandle.Init.ExternalTrigConv                   = ADC_SOFTWARE_START;
    adcHandle.Init.ExternalTrigConvEdge               = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adcHandle.Init.ConversionDataManagement           = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    adcHandle.Init.Resolution                         = ADC_RESOLUTION_16B;
    adcHandle.Init.ScanConvMode                       = ADC_SCAN_ENABLE;
    adcHandle.Init.EOCSelection                       = ADC_EOC_SINGLE_CONV;
    adcHandle.Init.LowPowerAutoWait                   = DISABLE;
    adcHandle.Init.ContinuousConvMode                 = ENABLE;
    adcHandle.Init.NbrOfConversion                    = 0x3;
    adcHandle.Init.DiscontinuousConvMode              = DISABLE;
    adcHandle.Init.ClockPrescaler                     = ADC_CLOCK_ASYNC_DIV8;
    if (HAL_ADC_Init(&adcHandle) != HAL_OK) {
        while (1)
            ;
    }

    ADC_MultiModeTypeDef multiMode = { 0 };
    multiMode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&adcHandle, &multiMode) != HAL_OK) {
        while (1)
            ;
    }

    ADC_ChannelConfTypeDef channelConf = { 0 };
    channelConf.OffsetSignedSaturation = DISABLE;
    channelConf.Offset                 = 0x0;
    channelConf.OffsetNumber           = ADC_OFFSET_NONE;
    channelConf.SingleDiff             = ADC_SINGLE_ENDED;
    channelConf.SamplingTime           = ADC_SAMPLETIME_64CYCLES_5;
    channelConf.Rank                   = ADC_REGULAR_RANK_1;
    channelConf.Channel                = ADC_CHANNEL_4;
    if (HAL_ADC_ConfigChannel(&adcHandle, &channelConf) != HAL_OK) {
        while (1)
            ;
    }

    channelConf.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&adcHandle, &channelConf) != HAL_OK) {
        while (1)
            ;
    }

    channelConf.Rank    = ADC_REGULAR_RANK_3;
    channelConf.Channel = ADC_CHANNEL_10;
    if (HAL_ADC_ConfigChannel(&adcHandle, &channelConf) != HAL_OK) {
        while (1)
            ;
    }

    HAL_ADCEx_Calibration_Start(&adcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);

    adc2Handle.Instance = ADC2;

    adc2Handle.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_4;
    adc2Handle.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
    adc2Handle.Init.ConversionDataManagement           = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    adc2Handle.Init.Overrun                            = ADC_OVR_DATA_OVERWRITTEN;
    adc2Handle.Init.LeftBitShift                       = ADC_LEFTBITSHIFT_NONE;
    adc2Handle.Init.OversamplingMode                   = ENABLE;
    adc2Handle.Init.Oversampling.Ratio                 = 0x10;
    adc2Handle.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
    adc2Handle.Init.ExternalTrigConv                   = ADC_SOFTWARE_START;
    adc2Handle.Init.ExternalTrigConvEdge               = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc2Handle.Init.ScanConvMode                       = ADC_SCAN_ENABLE;
    adc2Handle.Init.EOCSelection                       = ADC_EOC_SINGLE_CONV;
    adc2Handle.Init.LowPowerAutoWait                   = DISABLE;
    adc2Handle.Init.ContinuousConvMode                 = ENABLE;
    adc2Handle.Init.Resolution                         = ADC_RESOLUTION_16B;
    adc2Handle.Init.NbrOfConversion                    = 0x2;
    adc2Handle.Init.DiscontinuousConvMode              = DISABLE;
    adc2Handle.Init.ClockPrescaler                     = ADC_CLOCK_ASYNC_DIV8;
    if (HAL_ADC_Init(&adc2Handle) != HAL_OK) {
        while (1)
            ;
    }

    channelConf.SingleDiff              = ADC_SINGLE_ENDED;
    channelConf.OffsetSignedSaturation  = DISABLE;
    channelConf.OffsetNumber            = ADC_OFFSET_NONE;
    channelConf.Offset                  = 0x0;
    channelConf.SamplingTime            = ADC_SAMPLETIME_64CYCLES_5;
    channelConf.Rank                    = ADC_REGULAR_RANK_1;
    channelConf.Channel                 = ADC_CHANNEL_9;
    if (HAL_ADC_ConfigChannel(&adc2Handle, &channelConf) != HAL_OK) {
        while (1)
            ;
    }

    channelConf.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&adc2Handle, &channelConf) != HAL_OK) {
        while (1)
            ;
    }

    HAL_ADCEx_Calibration_Start(&adc2Handle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
}
