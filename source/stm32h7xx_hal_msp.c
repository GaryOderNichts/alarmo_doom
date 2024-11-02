#include "main.h"

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
}

void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};
    GPIO_InitTypeDef gpio_init_structure = { 0 };
    HAL_StatusTypeDef ret = HAL_OK;

    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
    RCC_PeriphCLKInitStruct.FmcClockSelection    = RCC_FMCCLKSOURCE_CLKP;
    ret = HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
    if (ret != HAL_OK) {
        while (1)
            ;
    }

    __HAL_RCC_FMC_CLK_ENABLE();

    gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio_init_structure.Alternate = GPIO_AF12_FMC;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_12;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio_init_structure.Alternate = GPIO_AF12_FMC;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_15 | GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_7;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio_init_structure.Alternate = GPIO_AF12_FMC;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOE, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_15 | GPIO_PIN_14;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio_init_structure.Alternate = GPIO_AF12_FMC;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &gpio_init_structure);

    gpio_init_structure.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_1 | GPIO_PIN_0;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio_init_structure.Alternate = GPIO_AF12_FMC;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &gpio_init_structure);

    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio_init_structure.Alternate = GPIO_AF9_FMC;
    gpio_init_structure.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);

    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
    gpio_init_structure.Alternate = GPIO_AF9_FMC;
    gpio_init_structure.Pin = GPIO_PIN_7;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOC, &gpio_init_structure);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef gpioInit = { 0 };

    if (hadc->Instance == ADC2) {
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        gpioInit.Pull = GPIO_NOPULL;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pin  = GPIO_PIN_0;
        HAL_GPIO_Init(GPIOB, &gpioInit);

        dma2Handle.Instance                 = DMA1_Stream2;
        dma2Handle.Init.Priority            = DMA_PRIORITY_LOW;
        dma2Handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        dma2Handle.Init.Request             = DMA_REQUEST_ADC2;
        dma2Handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        dma2Handle.Init.PeriphInc           = DMA_PINC_DISABLE;
        dma2Handle.Init.MemInc              = DMA_MINC_ENABLE;
        dma2Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        dma2Handle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        dma2Handle.Init.Mode                = DMA_CIRCULAR;
        if (HAL_DMA_Init(&dma2Handle) != HAL_OK) {
            while (1)
                ;
        }

        __HAL_LINKDMA(hadc, DMA_Handle, dma2Handle);
    } else if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        gpioInit.Pull = GPIO_NOPULL;
        gpioInit.Mode = GPIO_MODE_ANALOG;
        gpioInit.Pin  = GPIO_PIN_4 | GPIO_PIN_0;
        HAL_GPIO_Init(GPIOC, &gpioInit);

        dmaHandle.Instance                 = DMA1_Stream1;
        dmaHandle.Init.Priority            = DMA_PRIORITY_LOW;
        dmaHandle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        dmaHandle.Init.Request             = DMA_REQUEST_ADC1;
        dmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        dmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
        dmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
        dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        dmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        dmaHandle.Init.Mode                = DMA_CIRCULAR;
        if (HAL_DMA_Init(&dmaHandle) != HAL_OK) {
            while (1)
                ;
        }

        __HAL_LINKDMA(hadc, DMA_Handle, dmaHandle);
    }
}
