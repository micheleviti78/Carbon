/**
 ******************************************************************************
 * @file           main.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2022
 * @brief          CM7 main program body
 ******************************************************************************
 * @attention
 * Copyright (c) 2022 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <diag.hpp>
#include <error.hpp>
#include <main.hpp>
#include <pin.hpp>
#include <sdram.hpp>
#include <systime.hpp>

#include <cstring>
#include <fifo.hpp>

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

// static uint32_t sdram_buf __attribute__((aligned(4),
// section(".sdram_bank2")));

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
struct TestStruct {
    uint8_t var1{0};
    uint16_t var2{0};
};
#pragma pack(pop)

static constexpr auto dataAlignment = uint32_t{4};
static constexpr auto OneByteDataAlignment = uint32_t{1};
static constexpr auto dataElements = uint32_t{4};
static constexpr auto OneByteDataElements = uint32_t{200};

static TestStruct data[128];
static uint32_t dataPtr = reinterpret_cast<uint32_t>(&data[0]);

static uint8_t OneByteData[OneByteDataElements + 1];
static uint32_t OneByteDataPtr = reinterpret_cast<uint32_t>(&OneByteData[0]);

static void SystemClock_Config(void);

#define GET_HAL_VERSION_MAIN ((HAL_GetHalVersion() >> 24) & 0xFFUL)
#define GET_HAL_VERSION_SUB1 ((HAL_GetHalVersion() >> 16) & 0xFFUL)
#define GET_HAL_VERSION_SUB2 ((HAL_GetHalVersion() >> 8) & 0xFFUL)
#define GET_HAL_VERSION_RC (HAL_GetHalVersion() & 0xFFUL)

void _putchar(char ch);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    int32_t timeout;
    /* Wait until CPU2 boots and enters in stop mode or timeout*/
    timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0))
        ;

    if (timeout < 0) {
        Error_Handler();
    }

    /*activate cache*/
    SCB_EnableICache();

    SCB_EnableDCache();

    /*HAL low level init*/
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* init DIAG*/
    init_diag();

    /*init SDRAM*/
    init_sdram();

    /*init timer*/
    low_level_system_time();

    /* Initialize Pin needed by the Error function */
    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_ORANGE);

    /* When system initialization is finished, Cortex-M7 will release Cortex-M4
    by means of HSEM notification */
    /*HW semaphore Clock enable*/
    __HAL_RCC_HSEM_CLK_ENABLE();
    /*Take HSEM */
    HAL_HSEM_FastTake(HSEM_ID_0);
    /*Release HSEM in order to notify the CPU2(CM4)*/
    HAL_HSEM_Release(HSEM_ID_0, 0);
    /* wait until CPU2 wakes up from stop mode */
    timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0))
        ;
    if (timeout < 0) {
        Error_Handler();
    }

    RAW_DIAG("Initialization complete");
    RAW_DIAG("Newlib version %d.%d.%d", __NEWLIB__, __NEWLIB_MINOR__,
             __NEWLIB_PATCHLEVEL__);
    RAW_DIAG("HAL version %d.%d.%d.%d", GET_HAL_VERSION_MAIN,
             GET_HAL_VERSION_SUB1, GET_HAL_VERSION_SUB2, GET_HAL_VERSION_RC);

    /* Infinite loop */

    Buffer<TestStruct, dataAlignment> buffer{dataPtr + 1, 90};

    Fifo<TestStruct, dataAlignment, DummyLock, dataElements> fifo{buffer};

    TestStruct structWrite;
    TestStruct structRead;
    unsigned i;

    for (i = 0; i < dataElements + 1; i++) {
        structWrite.var1 = i + 1;
        structWrite.var2 = i * 10 + 3;

        if (!fifo.push(structWrite)) {
            RAW_DIAG("Overflow at %u", i);
        }
    }

    for (i = 0; i < dataElements + 1; i++) {
        if (!fifo.pop(structRead)) {
            RAW_DIAG("Underflow at %u", i);
        } else {
            if ((structRead.var1 != i + 1) || (structRead.var2 != i * 10 + 3)) {
                RAW_DIAG("read wrong data");
            }
        }
    }

    for (i = 0; i < dataElements + 1; i++) {
        structWrite.var1 = i + 67;
        structWrite.var2 = i * 10 + 954;

        if (!fifo.push(structWrite)) {
            RAW_DIAG("Overflow at %u", i);
        }
    }

    if (!fifo.pop(structRead)) {
        RAW_DIAG("Underflow at %u", i);
    } else {
        if ((structRead.var1 != 0 + 67) || (structRead.var2 != 0 * 10 + 954)) {
            RAW_DIAG("read wrong data");
        }
    }

    structWrite.var1 = 0xA5;
    structWrite.var2 = 65000;

    if (!fifo.push(structWrite)) {
        RAW_DIAG("Overflow at %u", i);
    }

    for (i = 1; i < dataElements; i++) {
        if (!fifo.pop(structRead)) {
            RAW_DIAG("Underflow at %u", i);
        } else {
            if ((structRead.var1 != i + 67) ||
                (structRead.var2 != i * 10 + 954)) {
                RAW_DIAG("read wrong data");
            }
        }
    }

    if (!fifo.pop(structRead)) {
        RAW_DIAG("Underflow at %u", i);
    } else {
        if ((structRead.var1 != 0xA5) || (structRead.var2 != 65000)) {
            RAW_DIAG("read wrong data");
        }
    }

    Buffer<uint8_t, OneByteDataAlignment> oneByteBuffer{
        OneByteDataPtr, OneByteDataElements + 1};

    Fifo<uint8_t, OneByteDataAlignment, DummyLock, OneByteDataElements>
        oneByteFifo{oneByteBuffer};

    const char *text1 = "test text numero 1. oggi siamo stati alla mostra del "
                        "carro agricolo\r\n";

    const char *text2 =
        "test text numero 2. oggi abbiamo visto i buoi chianini\r\n";

    const char *text3 = "test text numero 3. E' stata una bella giornata\r\n";

    const char *text4 = "test text numero 4. Sono stato benissimo, posto "
                        "incantevole e bellissimo panorama\r\n";

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                     std::strlen(text1));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                     std::strlen(text2));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                     std::strlen(text3));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                     std::strlen(text4));

    uint8_t ch;

    for (i = 0; i < std::strlen(text1); i++) {
        oneByteFifo.pop(ch);
        _putchar(ch);
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                     std::strlen(text4));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                     std::strlen(text1));

    while (oneByteFifo.pop(ch)) {
        _putchar(ch);
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                     std::strlen(text1));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                     std::strlen(text2));

    while (oneByteFifo.pop(ch)) {
        _putchar(ch);
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                     std::strlen(text3));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                     std::strlen(text4));

    while (oneByteFifo.pop(ch)) {
        _putchar(ch);
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                     std::strlen(text1));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                     std::strlen(text2));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                     std::strlen(text3));

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                     std::strlen(text4));

    while (oneByteFifo.pop(ch)) {
        _putchar(ch);
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                     std::strlen(text4));

    i = 100000;

    while (i > 0) {
        oneByteFifo.pop(ch);

        _putchar(ch);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                         std::strlen(text4));
        i--;
    }

    i = 100000;

    while (i > 0) {
        oneByteFifo.pop(ch);

        _putchar(ch);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                         std::strlen(text3));
        i--;
    }

    i = 100000;

    while (i > 0) {
        oneByteFifo.pop(ch);

        _putchar(ch);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                         std::strlen(text2));
        i--;
    }

    i = 100000;

    while (i > 0) {
        oneByteFifo.pop(ch);

        _putchar(ch);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                         std::strlen(text1));
        i--;
    }

    while (oneByteFifo.pop(ch)) {
        _putchar(ch);
    }

    while (1) {
        HAL_Delay(1000);
        BSP_LED_Toggle(LED_GREEN);
    }
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
 *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus
 *                 matrix Clocks)
 *            AHB Prescaler                  = 2
 *            D1 APB3 Prescaler              = 2
 *                (APB3 Clock  100MHz)
 *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
 *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
 *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
 *            HSE Frequency(Hz)              = 25000000 PLL_M = 5
 *            PLL_N                          = 160
 *            PLL_P                          = 2
 *            PLL_Q                          = 4
 *            PLL_R                          = 2
 *            VDD(V)                         = 3.3
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;

    /*!< Supply configuration update enable */
    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

    /* The voltage scaling allows optimizing the power consumption when the
       device is clocked below the maximum system frequency, to update the
       voltage scaling value regarding system frequency refer to product
       datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLQ = 4;

    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK) {
        Error_Handler();
    }

    /* Select PLL as system clock source and configure  bus clocks dividers */
    RCC_ClkInitStruct.ClockType =
        (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 |
         RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1);

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
    if (ret != HAL_OK) {
        Error_Handler();
    }

    /*
    Note : The activation of the I/O Compensation Cell is recommended with
    communication  interfaces (GPIO, SPI, FMC, QSPI ...)  when  operating at
    high frequencies(please refer to product datasheet) The I/O Compensation
    Cell activation  procedure requires :
          - The activation of the CSI clock
          - The activation of the SYSCFG clock
          - Enabling the I/O Compensation Cell : setting bit[0] of register
    SYSCFG_CCCSR

            To do this please uncomment the following code
  */

    __HAL_RCC_CSI_ENABLE();

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_EnableCompensationCell();
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */

#ifdef __cplusplus
}
#endif