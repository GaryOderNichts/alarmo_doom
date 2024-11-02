/**
 * Input code for the Nintendo Alarmo.
 * Created in 2024 by GaryOderNichts.
 */
#pragma once

#include <stdint.h>

enum {
    BUTTON_NOTIFICATION = (1u << 0),
    BUTTON_BACK         = (1u << 1),
    BUTTON_DIAL         = (1u << 2),
};

void INPUT_Init(void);

uint32_t INPUT_GetButtons(void);

float INPUT_GetDial(void);
