#pragma once

#include <stm32h7xx_hal.h>

extern SRAM_HandleTypeDef fmcHandle;
extern TIM_HandleTypeDef tim3Handle;
extern MDMA_HandleTypeDef mdmaHandle;
extern ADC_HandleTypeDef adcHandle;
extern DMA_HandleTypeDef dmaHandle;
extern ADC_HandleTypeDef adc2Handle;
extern DMA_HandleTypeDef dma2Handle;

extern uint8_t doom1_wad_unc[];
extern uint32_t doom1_wad_unc_size;

void sleep_ms(uint32_t ms);
uint32_t get_systime(void);
