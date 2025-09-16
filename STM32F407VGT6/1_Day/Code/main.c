/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Prateek by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>

// Base addresses
#define PERIPH_BASE        0x40000000U
#define PERIPH_BB_BASE     0x42000000U

#define AHB1ENR_OFFSET     0x30U
#define RCC_BASE           (PERIPH_BASE + 0x23800U)   // RCC base address
#define RCC_AHB1ENR        (*(volatile uint32_t *)(RCC_BASE + AHB1ENR_OFFSET))

#define GPIOD_BASE         (PERIPH_BASE + 0x20C00U)   // GPIOD base address
#define GPIOD_MODER        (*(volatile uint32_t *)(GPIOD_BASE + 0x00U))

// Macro to get bit-band alias address for a peripheral bit
#define BITBAND_PERI(addr, bit) \
    ((volatile uint32_t *)(PERIPH_BB_BASE + (((uint32_t)(addr) - PERIPH_BASE) * 32U) + ((bit) * 4U)))

// Simple delay
static void delay(volatile uint32_t t) {
    while (t--) {
        __asm volatile ("nop");
    }
}

int main(void)
{
    // 1. Enable GPIOD clock (bit 3 in RCC_AHB1ENR)
    RCC_AHB1ENR |= (1U << 3);

    // 2. Configure pins 12, 13, 14, 15 as outputs
    GPIOD_MODER &= ~((3U << (12 * 2)) | (3U << (13 * 2)) | (3U << (14 * 2)) | (3U << (15 * 2)));
    GPIOD_MODER |=  ((1U << (12 * 2)) | (1U << (13 * 2)) | (1U << (14 * 2)) | (1U << (15 * 2)));

    // 3. Store bit-band addresses for LEDs in an array
    volatile uint32_t *leds[4] = {
        BITBAND_PERI(GPIOD_BASE + 0x14U, 12),
        BITBAND_PERI(GPIOD_BASE + 0x14U, 13),
        BITBAND_PERI(GPIOD_BASE + 0x14U, 14),
        BITBAND_PERI(GPIOD_BASE + 0x14U, 15)
    };

    // 4. Loop forever, lighting each LED sequentially
    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            // Turn all LEDs OFF first
            *leds[0] = 0;
            *leds[1] = 0;
            *leds[2] = 0;
            *leds[3] = 0;

            // Turn current LED ON
            *leds[i] = 1;

            delay(2000000); // Adjust for visible speed
        }
    }
}
