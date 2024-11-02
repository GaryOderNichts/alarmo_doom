/**
 * Input code for the Nintendo Alarmo.
 * Created in 2024 by GaryOderNichts.
 */
#include "input.h"
#include <main.h>
#include <stm32h7xx_hal.h>

ALIGN_32BYTES (__IO uint16_t   adcConvertedValues[3]);
ALIGN_32BYTES (__IO uint16_t   adcConvertedValues2[2]);

void INPUT_Init(void)
{
    // Start reading adc data into buffers
    HAL_ADC_Start_DMA(&adcHandle, (uint32_t *)adcConvertedValues, 3);
    HAL_ADC_Start_DMA(&adc2Handle, (uint32_t *)adcConvertedValues2, 2);
}

uint32_t INPUT_GetButtons(void)
{
    uint32_t buttons = 0;

    if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_5) == GPIO_PIN_RESET) {
        buttons |= BUTTON_NOTIFICATION;
    }

    if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6) == GPIO_PIN_RESET) {
        buttons |= BUTTON_BACK;
    }

    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_RESET) {
        buttons |= BUTTON_DIAL;
    }

    return buttons;
}

float INPUT_GetDial(void)
{
    // Invalidate adc buffers
    SCB_InvalidateDCache_by_Addr((void *)adcConvertedValues, sizeof(adcConvertedValues));
    SCB_InvalidateDCache_by_Addr((void *)adcConvertedValues2, sizeof(adcConvertedValues2));

    float val1 = (float) adcConvertedValues[0] / 65535.0f;
    float val2 = (float) adcConvertedValues2[0] / 65535.0f;

    float radians = atan2f(val1 - 0.5f, val2 - 0.5f);
    float degrees = (radians * 180.0f) / 3.141593f + 180.0f;

    return degrees;
}
