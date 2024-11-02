/**
 * ST7789 LCD code for the Nintendo Alarmo.
 * Created in 2024 by GaryOderNichts.
 */
#include "lcd.h"
#include "main.h"
#include <stm32h7xx_hal.h>

#define USE_DMA 1

#define LCD_COMMAND (*(volatile uint8_t *) 0xc0000000)

#define LCD_RS_PIN (6)
#define LCD_DATA_ADDRESS (0xc0000000lu + (1lu << (LCD_RS_PIN + 1)))
#define LCD_DATA8 (*(volatile uint8_t *)LCD_DATA_ADDRESS)
#define LCD_DATA16 (*(volatile uint16_t *)LCD_DATA_ADDRESS)

static void LCD_Select(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
}

static void LCD_Deselect(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
}

static void LCD_WriteCommand(uint8_t cmd)
{
    LCD_COMMAND = cmd;
}

static void LCD_WriteData(uint8_t data)
{
    LCD_DATA8 = data;
}

static void LCD_InitSequence(void)
{
    LCD_Select();
    LCD_WriteCommand(0x11);
    LCD_Deselect();

    HAL_Delay(120);

    LCD_Select();
    LCD_WriteCommand(0x36);
    LCD_WriteData(0x0);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0x21);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0x3a);
    LCD_WriteData(0x6);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xb7);
    LCD_WriteData(0x37);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xbb);
    LCD_WriteData(0x14);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xc0);
    LCD_WriteData(0x2c);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xc2);
    LCD_WriteData(0x1);
    LCD_WriteData(0xff);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xc3);
    LCD_WriteData(0x19);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xc6);
    LCD_WriteData(0x17);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xd0);
    LCD_WriteData(0xa4);
    LCD_WriteData(0xb3);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xe0);
    LCD_WriteData(0xa0);
    LCD_WriteData(0x3);
    LCD_WriteData(0x6);
    LCD_WriteData(0x7);
    LCD_WriteData(0x6);
    LCD_WriteData(0x24);
    LCD_WriteData(0x25);
    LCD_WriteData(0x33);
    LCD_WriteData(0x3d);
    LCD_WriteData(0x27);
    LCD_WriteData(0x14);
    LCD_WriteData(0x14);
    LCD_WriteData(0x28);
    LCD_WriteData(0x2f);
    LCD_Deselect();

    LCD_Select();
    LCD_WriteCommand(0xe1);
    LCD_WriteData(0xa0);
    LCD_WriteData(0x3);
    LCD_WriteData(0x6);
    LCD_WriteData(0x7);
    LCD_WriteData(0x6);
    LCD_WriteData(0x24);
    LCD_WriteData(0x25);
    LCD_WriteData(0x33);
    LCD_WriteData(0x3d);
    LCD_WriteData(0x27);
    LCD_WriteData(0x14);
    LCD_WriteData(0x14);
    LCD_WriteData(0x28);
    LCD_WriteData(0x2f);
    LCD_Deselect();
}

static void LCD_FRCTR2(uint8_t p1, uint8_t p2)
{
    LCD_Select();

    LCD_WriteCommand(0xc6);
    LCD_WriteData((p1 << 5) | (p2 & 0x1f));

    LCD_Deselect();
}

static void LCD_INVON(void)
{
    LCD_Select();

    LCD_WriteCommand(0x21);

    LCD_Deselect();
}

static void LCD_MADCTL(uint8_t p1, uint8_t p2, uint8_t p3, uint8_t p4)
{
    LCD_Select();

    LCD_WriteCommand(0x36);
    LCD_WriteData((p2 << 6) | (p1 << 7) | (p3 << 5) | (p4 << 3));

    LCD_Deselect();
}

static void LCD_DISPOFF(void)
{
    LCD_Select();

    LCD_WriteCommand(0x28);

    LCD_Deselect();
}

void LCD_DISPON(void)
{
    LCD_Select();

    LCD_WriteCommand(0x29);

    LCD_Deselect();
}

static void LCD_SLPOUT(void)
{
    LCD_Select();

    LCD_WriteCommand(0x11);

    LCD_Deselect();
}

static void LCD_COLMOD(uint8_t p1, uint8_t p2)
{
    LCD_Select();

    LCD_WriteCommand(0x3a);
    LCD_WriteData(p2 | (p1 << 4));

    LCD_Deselect();
}

static void LCD_RAMCTRL(uint8_t p2, uint8_t p3, uint8_t p4, uint8_t p5, uint8_t p6)
{
    LCD_Select();

    LCD_WriteCommand(0xb0);
    LCD_WriteData(p3 | (p2 << 4));
    LCD_WriteData(p5 | (p4 << 3) | (p6 << 2) | 0xc0);

    LCD_Deselect();
}

static void LCD_CASET(uint16_t p1, uint16_t p2)
{
    LCD_Select();

    LCD_WriteCommand(0x2a);
    LCD_WriteData((uint8_t)(p1 >> 8));
    LCD_WriteData((uint8_t)(p1));
    LCD_WriteData((uint8_t)(p2 >> 8));
    LCD_WriteData((uint8_t)(p2));

    LCD_Deselect();
}

static void LCD_RASET(uint16_t p1, uint16_t p2)
{
    LCD_Select();

    LCD_WriteCommand(0x2b);
    LCD_WriteData((uint8_t)(p1 >> 8));
    LCD_WriteData((uint8_t)(p1));
    LCD_WriteData((uint8_t)(p2 >> 8));
    LCD_WriteData((uint8_t)(p2));

    LCD_Deselect();
}

void LCD_RDID(uint32_t* outId)
{
    LCD_Select();
    LCD_WriteCommand(0x04); // RDID
    *outId |= LCD_DATA8 << 24;
    *outId |= LCD_DATA8 << 16;
    *outId |= LCD_DATA8 << 8;
    *outId |= LCD_DATA8;
    LCD_Deselect();
}

void LCD_Init(void)
{
    HAL_MDMA_Abort(&mdmaHandle);

    LCD_Deselect();

    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_Delay(120);

    LCD_InitSequence();
    LCD_FRCTR2(0, 0x17);
    LCD_INVON();
    LCD_MADCTL(0, 0, 0, 1);
    LCD_DISPOFF();
    LCD_SLPOUT();
    LCD_COLMOD(5, 6);
    LCD_RAMCTRL(0, 0, 0, 0, 0);
    LCD_CASET(0, SCREEN_HEIGHT - 1);
    LCD_RASET(0, SCREEN_WIDTH - 1);
}

static void LCD_DrawData(const uint8_t *source, uint32_t size)
{
#if USE_DMA == 1
    // flush cache for dma
    SCB_CleanDCache_by_Addr((uint32_t *)source, size);

    uint32_t offset = 0;
    while (size > 0) {
        uint32_t to_copy = size;
        if (to_copy > 65536U) {
            to_copy = 65536U;
        }

        if (HAL_MDMA_Start(&mdmaHandle, (uint32_t)source + offset, (uint32_t) &LCD_DATA16, to_copy, 1) != HAL_OK) {
            while (1)
                ;            
        }

        // TODO instead of polling, perform irq driven transfers
        if (HAL_MDMA_PollForTransfer(&mdmaHandle, HAL_MDMA_FULL_TRANSFER, 1000) != HAL_OK) {
            while (1)
                ;
        }

        size -= to_copy;
        offset += to_copy;
    }
#else // Software
    for (uint32_t i = 0; i < size; i += 2) {
        LCD_DATA16 = (source[i] << 8) | source[i + 1];
    }
#endif
}

void LCD_DrawRect(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t size = y1 * x1 + ((y1 * x1) % 2);

    uint32_t rowsize = size;
    if (rowsize > SCREEN_HEIGHT) {
        rowsize = SCREEN_HEIGHT;
    }

    static uint8_t color_buffer[SCREEN_HEIGHT * 3];
    for (uint32_t i = 0; i < rowsize; i++) {
        color_buffer[(i*3)+2] = r;
        color_buffer[(i*3)+1] = g;
        color_buffer[(i*3)  ] = b;
    }

    // flush cache for dma
    SCB_CleanDCache_by_Addr((uint32_t *)&color_buffer[0], rowsize);

    LCD_RASET(SCREEN_WIDTH - (x0 + x1), SCREEN_WIDTH - x0 - 1);
    LCD_CASET(y0, (y1 + y0) - 1);

    LCD_Select();
    LCD_WriteCommand(0x2c);

    while (size > 0) {
        uint32_t to_copy = size;
        if (to_copy > SCREEN_HEIGHT) {
            to_copy = SCREEN_HEIGHT;
        }

        LCD_DrawData(&color_buffer[0], to_copy * 3);

        size -= to_copy;
    }

    LCD_Deselect();
}

void LCD_DrawScreenBuffer(const uint8_t *buffer, uint32_t size)
{
    // flush cache for dma
    SCB_CleanDCache_by_Addr((uint32_t *)buffer, size);

    LCD_RASET(0, SCREEN_WIDTH - 1);
    LCD_CASET(0, SCREEN_HEIGHT - 1);

    LCD_Select();
    LCD_WriteCommand(0x2c);
    LCD_DrawData(buffer, size);
    LCD_Deselect();
}

void LCD_SetBrightness(float brightness)
{
    __HAL_TIM_SET_COMPARE(&tim3Handle, TIM_CHANNEL_4, brightness * tim3Handle.Init.Period);
    HAL_TIM_PWM_Start(&tim3Handle, TIM_CHANNEL_4);
}
