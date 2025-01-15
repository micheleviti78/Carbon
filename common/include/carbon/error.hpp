/**
 ******************************************************************************
 * @file           error.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 error hanndler and type
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

#pragma once

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

// Define error groups
enum class ErrorGroupType : uint16_t {
    None = 0,
    Network,
    Modbus,
    FileSystem,
    Hardware,
    Software
};

// Define specific errors
enum class ErrorType : uint16_t {
    None = 0,
    NetworkTimeout,
    NetworkUnavailable,
    NetworkConnectionFailed,
    NetworkInvalidIP,
    ModbusIPNotSet,
    ModbusRequestFailed,
    ModbusResponseFailed,
    FileNotFound,
    DiskFull,
    SensorFailure,
    Internal
};

// Class to hold an error and its associated group
class Error {
public:
    // default constructor
    constexpr Error() : group_(ErrorGroupType::None), error_(ErrorType::None){};

    // Constructor to initialize the error and its group
    constexpr Error(ErrorGroupType group, ErrorType error)
        : group_(group), error_(error) {}

    // Retrieve the error group
    constexpr ErrorGroupType group() const { return group_; }

    // Retrieve the specific error
    constexpr ErrorType error() const { return error_; }

    // Overload the equality operator
    constexpr bool operator==(const Error &other) const {
        return group_ == other.group_ && error_ == other.error_;
    }

    // Overload the inequality operator
    constexpr bool operator!=(const Error &other) const {
        return !(*this == other);
    }

    // Cast to bool operator
    explicit constexpr operator bool() const {
        return group_ != ErrorGroupType::None || error_ != ErrorType::None;
    }

private:
    ErrorGroupType group_;
    ErrorType error_;
};

static_assert(sizeof(Error) == 4);

constexpr Error Success{};

constexpr Error InternalError(ErrorGroupType::Software, ErrorType::Internal);

constexpr Error InternalHardwareError(ErrorGroupType::Hardware,
                                      ErrorType::Internal);

constexpr Error NetworkConnectionFailed(ErrorGroupType::Network,
                                        ErrorType::NetworkConnectionFailed);

constexpr Error NetworkInvalidIP(ErrorGroupType::Network,
                                 ErrorType::NetworkInvalidIP);

constexpr Error ModbusIPNotSet(ErrorGroupType::Modbus,
                               ErrorType::ModbusIPNotSet);

constexpr Error ModbusRequestFailed(ErrorGroupType::Modbus,
                                    ErrorType::ModbusRequestFailed);

constexpr Error ModbusResponseFailed(ErrorGroupType::Modbus,
                                     ErrorType::ModbusResponseFailed);