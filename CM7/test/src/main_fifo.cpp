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
#include <fifo.hpp>
#include <pin.hpp>

#include <stm32h7xx_hal.h>

#include <cmath>
#include <cstring>

class FifoTest {
public:
    FifoTest() = default;
    void test();
    uint8_t bitFieldConverter(uint32_t n) {
        uint32_t p{1};
        for (unsigned i = 0; i < n; i++) {
            p = p * 2;
        }
        return static_cast<uint8_t>((p - 1) & 0x000000FF);
    }
};

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

static Fifo<TestStruct, dataAlignment, DummyLock, dataElements> fifo;

static Fifo<uint8_t, OneByteDataAlignment, DummyLock, OneByteDataElements>
    oneByteFifo;

static const char *text1 =
    "test text numero 1. oggi siamo stati alla mostra del "
    "carro agricolo\r\n";

static const char *text2 =
    "test text numero 2. oggi abbiamo visto i buoi chianini\r\n";

static const char *text3 =
    "test text numero 3. E' stata una bella giornata\r\n";

static const char *text4 = "test text numero 4. Sono stato benissimo, posto "
                           "incantevole e bellissimo panorama\r\n";

static const char *text_to_success_to_fill_buffer =
    "aaaaaaaaaaaaaaaaaaaaaaaa\r\n";
static const char *text_to_fail_to_fill_buffer =
    "aaaaaaaaaaaaaaaaaaaaaaaaa\r\n";

static DummyLock dummyLock;

#define GET_HAL_VERSION_MAIN ((HAL_GetHalVersion() >> 24) & 0xFFUL)
#define GET_HAL_VERSION_SUB1 ((HAL_GetHalVersion() >> 16) & 0xFFUL)
#define GET_HAL_VERSION_SUB2 ((HAL_GetHalVersion() >> 8) & 0xFFUL)
#define GET_HAL_VERSION_RC (HAL_GetHalVersion() & 0xFFUL)

void _putchar(char ch);

void mainThread(const void * /*argument*/) {}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    RAW_DIAG("Initialization complete");
    RAW_DIAG("Newlib version %d.%d.%d", __NEWLIB__, __NEWLIB_MINOR__,
             __NEWLIB_PATCHLEVEL__);
    RAW_DIAG("HAL version %d.%d.%d.%d", GET_HAL_VERSION_MAIN,
             GET_HAL_VERSION_SUB1, GET_HAL_VERSION_SUB2, GET_HAL_VERSION_RC);

    /* Infinite loop */

    if (!(fifo.init((dataPtr + 1), 90))) {
        RAW_DIAG("init fifo failed");
        while (1)
            ;
    }

    if (!(oneByteFifo.init(OneByteDataPtr, OneByteDataElements + 1))) {
        RAW_DIAG("init oneByteFifo failed");
        while (1)
            ;
    }

    TestStruct structWrite;
    TestStruct structRead;
    unsigned i;

    for (i = 0; i < dataElements + 1; i++) {
        structWrite.var1 = i + 1;
        structWrite.var2 = i * 10 + 3;

        if (!fifo.push(structWrite, dummyLock)) {
            RAW_DIAG("Overflow at %u", i);
        }
    }

    for (i = 0; i < dataElements + 1; i++) {
        if (!fifo.pop(structRead, dummyLock)) {
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

        if (!fifo.push(structWrite, dummyLock)) {
            RAW_DIAG("Overflow at %u", i);
        }
    }

    if (!fifo.pop(structRead, dummyLock)) {
        RAW_DIAG("Underflow at %u", i);
    } else {
        if ((structRead.var1 != 0 + 67) || (structRead.var2 != 0 * 10 + 954)) {
            RAW_DIAG("read wrong data");
        }
    }

    structWrite.var1 = 0xA5;
    structWrite.var2 = 65000;

    if (!fifo.push(structWrite, dummyLock)) {
        RAW_DIAG("Overflow at %u", i);
    }

    for (i = 1; i < dataElements; i++) {
        if (!fifo.pop(structRead, dummyLock)) {
            RAW_DIAG("Underflow at %u", i);
        } else {
            if ((structRead.var1 != i + 67) ||
                (structRead.var2 != i * 10 + 954)) {
                RAW_DIAG("read wrong data");
            }
        }
    }

    if (!fifo.pop(structRead, dummyLock)) {
        RAW_DIAG("Underflow at %u", i);
    } else {
        if ((structRead.var1 != 0xA5) || (structRead.var2 != 65000)) {
            RAW_DIAG("read wrong data");
        }
    }

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                         std::strlen(text1), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                         std::strlen(text2), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                         std::strlen(text3), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                         std::strlen(text4), dummyLock);

        uint8_t ch;

        for (i = 0; i < std::strlen(text1); i++) {
            oneByteFifo.pop(ch, dummyLock);
            _putchar(ch);
        }

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                         std::strlen(text4), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                         std::strlen(text1), dummyLock);

        while (oneByteFifo.pop(ch, dummyLock)) {
            _putchar(ch);
        }

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                         std::strlen(text1), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                         std::strlen(text2), dummyLock);

        while (oneByteFifo.pop(ch, dummyLock)) {
            _putchar(ch);
        }

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                         std::strlen(text3), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                         std::strlen(text4), dummyLock);

        while (oneByteFifo.pop(ch, dummyLock)) {
            _putchar(ch);
        }

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                         std::strlen(text1), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                         std::strlen(text2), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                         std::strlen(text3), dummyLock);

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                         std::strlen(text4), dummyLock);

        while (oneByteFifo.pop(ch, dummyLock)) {
            _putchar(ch);
        }

        oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                         std::strlen(text4), dummyLock);

        i = 100000;

        while (i > 0) {
            oneByteFifo.pop(ch, dummyLock);

            _putchar(ch);

            oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                             std::strlen(text4), dummyLock);
            i--;
        }

        i = 100000;

        while (i > 0) {
            oneByteFifo.pop(ch, dummyLock);

            _putchar(ch);

            oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                             std::strlen(text3), dummyLock);
            i--;
        }

        i = 100000;

        while (i > 0) {
            oneByteFifo.pop(ch, dummyLock);

            _putchar(ch);

            oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                             std::strlen(text2), dummyLock);
            i--;
        }

        i = 100000;

        while (i > 0) {
            oneByteFifo.pop(ch, dummyLock);

            _putchar(ch);

            oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                             std::strlen(text1), dummyLock);
            i--;
        }

        while (oneByteFifo.pop(ch, dummyLock)) {
            _putchar(ch);
        }

    FifoTest test;
    test.test();

    while (1) {
        HAL_Delay(1000);
        BSP_LED_Toggle(LED_GREEN);
    }
}

void FifoTest::test() {
    size_t expected_tail{0};
    size_t expected_head{0};
    uint8_t bitField{0};
    oneByteFifo.reset(dummyLock);
    RAW_DIAG("starts test internal index");

    RAW_DIAG("text_length 1 %d", std::strlen(text1));
    RAW_DIAG("text_length 2 %d", std::strlen(text2));
    RAW_DIAG("text_length 3 %d", std::strlen(text3));
    RAW_DIAG("text_length 4 %d", std::strlen(text4));
    RAW_DIAG("text_length text_to_fail_to_fill_buffer %d",
             std::strlen(text_to_fail_to_fill_buffer));
    RAW_DIAG("text_length text_to_success_to_fill_buffer %d",
             std::strlen(text_to_success_to_fill_buffer));

    if (oneByteFifo.bit_field_size_ != 26) {
        RAW_DIAG("error bit_field_size_ %lu", oneByteFifo.bit_field_size_);
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text1),
                     std::strlen(text1), dummyLock);

    expected_tail = std::strlen(text1);

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    for (unsigned i = 0; i < (expected_tail / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0xFF) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = bitFieldConverter(expected_tail % 8);

    if (oneByteFifo.tail_ready_[expected_tail / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u", expected_tail / 8u,
                 oneByteFifo.tail_ready_[expected_tail / 8u], bitField);
    }

    for (unsigned i = (expected_tail / 8u + 1); i < oneByteFifo.bit_field_size_;
         i++) {
        if (oneByteFifo.tail_ready_[i] != 0) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                     std::strlen(text2), dummyLock);

    expected_tail += std::strlen(text2);

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    for (unsigned i = 0; i < (expected_tail / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0xFF) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = bitFieldConverter(expected_tail % 8);

    if (oneByteFifo.tail_ready_[expected_tail / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u", expected_tail / 8u,
                 oneByteFifo.tail_ready_[expected_tail / 8u], bitField);
    }

    for (unsigned i = (expected_tail / 8u + 1); i < oneByteFifo.bit_field_size_;
         i++) {
        if (oneByteFifo.tail_ready_[i] != 0) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    oneByteFifo.push(reinterpret_cast<const uint8_t *>(text3),
                     std::strlen(text3), dummyLock);

    if (oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                         std::strlen(text4), dummyLock)) {
        RAW_DIAG("expected push to fail here");
    }

    expected_tail += std::strlen(text3);

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    for (unsigned i = 0; i < (expected_tail / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0xFF) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = bitFieldConverter(expected_tail % 8);

    if (oneByteFifo.tail_ready_[expected_tail / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u", expected_tail / 8u,
                 oneByteFifo.tail_ready_[expected_tail / 8u], bitField);
    }

    for (unsigned i = (expected_tail / 8u + 1); i < oneByteFifo.bit_field_size_;
         i++) {
        if (oneByteFifo.tail_ready_[i] != 0) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    /***************************************/

    if (oneByteFifo.push(
            reinterpret_cast<const uint8_t *>(text_to_fail_to_fill_buffer),
            std::strlen(text_to_fail_to_fill_buffer), dummyLock))
        RAW_DIAG("expected push to fail here");

    if (!oneByteFifo.push(
            reinterpret_cast<const uint8_t *>(text_to_success_to_fill_buffer),
            std::strlen(text_to_success_to_fill_buffer), dummyLock))
        RAW_DIAG("expected push to success here");

    expected_tail += std::strlen(text_to_success_to_fill_buffer);

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    if (oneByteFifo.tail_reserved_ != oneByteFifo.BUFFER_SIZE - 1) {
        RAW_DIAG("FIFO should be full %lu", oneByteFifo.tail_reserved_);
    }

    /***************************************/

    uint8_t ch;
    uint32_t index = std::strlen(text1);

    while (index > 0) {
        if (!oneByteFifo.pop(ch, dummyLock))
            RAW_DIAG("error pop %lu", index);
        index--;
    }

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    expected_head = std::strlen(text1);

    if (oneByteFifo.head_ != expected_head)
        RAW_DIAG("error head_: expected %lu, got %lu", expected_head,
                 oneByteFifo.head_);

    for (unsigned i = 0; i < (expected_head / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = ~bitFieldConverter(expected_head % 8u);

    if (oneByteFifo.tail_ready_[expected_head / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u", expected_head / 8u,
                 oneByteFifo.tail_ready_[expected_head / 8u], bitField);
    }

    index = std::strlen(text2);

    while (index > 0) {
        if (!oneByteFifo.pop(ch, dummyLock))
            RAW_DIAG("error pop %lu", index);
        index--;
    }

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    expected_head = +std::strlen(text2) + expected_head;

    if (oneByteFifo.head_ != expected_head)
        RAW_DIAG("error head_: expected %lu, got %lu", expected_head,
                 oneByteFifo.head_);

    for (unsigned i = 0; i < (expected_head / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = ~bitFieldConverter(expected_head % 8u);

    if (oneByteFifo.tail_ready_[expected_head / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u", expected_head / 8u,
                 oneByteFifo.tail_ready_[expected_head / 8u], bitField);
    }

    if (!oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                          std::strlen(text4), dummyLock)) {
        RAW_DIAG("expected push to success here");
    }

    expected_tail = std::strlen(text4) - 1;

    if (oneByteFifo.tail_reserved_ != expected_tail) {
        RAW_DIAG("error tail_reserved_ %lu", oneByteFifo.tail_reserved_);
    }

    RAW_DIAG("tail %lu, head %lu", oneByteFifo.tail_reserved_,
             oneByteFifo.head_);

    for (unsigned i = 0; i < (oneByteFifo.tail_reserved_ / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0xFF) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = bitFieldConverter(expected_tail % 8);

    if (oneByteFifo.tail_ready_[expected_tail / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u", expected_tail / 8u,
                 oneByteFifo.tail_ready_[expected_tail / 8u], bitField);
    }

    for (unsigned i = (expected_tail / 8u + 1); i < (oneByteFifo.head_ / 8u);
         i++) {
        if (oneByteFifo.tail_ready_[i] != 0) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = ~bitFieldConverter(oneByteFifo.head_ % 8u);

    if (oneByteFifo.tail_ready_[oneByteFifo.head_ / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u", expected_head / 8u,
                 oneByteFifo.tail_ready_[expected_head / 8u], bitField);
    }

    for (unsigned i = (oneByteFifo.head_ / 8u + 1);
         i < (oneByteFifo.bit_field_size_ - 1); i++) {
        if (oneByteFifo.tail_ready_[i] != 0xFF) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = bitFieldConverter(oneByteFifo.BUFFER_SIZE % 8);

    if (oneByteFifo.tail_ready_[oneByteFifo.bit_field_size_ - 1] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u",
                 oneByteFifo.bit_field_size_ - 1,
                 oneByteFifo.tail_ready_[oneByteFifo.bit_field_size_ - 1],
                 bitField);
    }

    /***************************************/

    index = 100u;

    while (index > 0) {
        if (!oneByteFifo.pop(ch, dummyLock))
            RAW_DIAG("error pop %lu", index);
        index--;
    }

    RAW_DIAG("tail %lu, head %lu", oneByteFifo.tail_reserved_,
             oneByteFifo.head_);

    if (!oneByteFifo.push(reinterpret_cast<const uint8_t *>(text4),
                          std::strlen(text4), dummyLock)) {
        RAW_DIAG("expected push to success here");
    }

    RAW_DIAG("tail %lu, head %lu", oneByteFifo.tail_reserved_,
             oneByteFifo.head_);

    if (!oneByteFifo.push(reinterpret_cast<const uint8_t *>(text2),
                          std::strlen(text2), dummyLock)) {
        RAW_DIAG("expected push to success here");
    }

    RAW_DIAG("tail %lu, head %lu", oneByteFifo.tail_reserved_,
             oneByteFifo.head_);

    for (unsigned i = 0; i < (oneByteFifo.tail_reserved_ / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0xFF) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = bitFieldConverter(oneByteFifo.tail_reserved_ % 8);

    if (oneByteFifo.tail_ready_[oneByteFifo.tail_reserved_ / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u",
                 oneByteFifo.tail_reserved_ / 8u,
                 oneByteFifo.tail_ready_[oneByteFifo.tail_reserved_ / 8u],
                 bitField);
    }

    for (unsigned i = (oneByteFifo.tail_reserved_ / 8u + 1);
         i < (oneByteFifo.head_ / 8u); i++) {
        if (oneByteFifo.tail_ready_[i] != 0) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = ~bitFieldConverter(oneByteFifo.head_ % 8u);

    if (oneByteFifo.tail_ready_[oneByteFifo.head_ / 8u] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u",
                 oneByteFifo.head_ / 8u,
                 oneByteFifo.tail_ready_[oneByteFifo.head_ / 8u], bitField);
    }

    for (unsigned i = (oneByteFifo.head_ / 8u + 1);
         i < (oneByteFifo.bit_field_size_ - 1); i++) {
        if (oneByteFifo.tail_ready_[i] != 0xFF) {
            RAW_DIAG("error tail_ready_[%d] = %u", i,
                     oneByteFifo.tail_ready_[i]);
        }
    }

    bitField = bitFieldConverter(oneByteFifo.BUFFER_SIZE % 8);

    if (oneByteFifo.tail_ready_[oneByteFifo.bit_field_size_ - 1] != bitField) {
        RAW_DIAG("error tail_ready_[%d] = %u, expected %u",
                 oneByteFifo.bit_field_size_ - 1,
                 oneByteFifo.tail_ready_[oneByteFifo.bit_field_size_ - 1],
                 bitField);
    }
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
