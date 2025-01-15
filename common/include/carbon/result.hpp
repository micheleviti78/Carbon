
/**
 ******************************************************************************
 * @file           result.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2025
 * @brief          simulate std::expected
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

#include <carbon/error.hpp>

// Template class to hold either a value or an error
// Simplified version for std::expected
template <typename T> class Result {
public:
    // Constructor for a successful result
    Result(const T &value)
        : success_(true), value_(value),
          errorInfo_(ErrorGroupType::None, ErrorType::None) {}

    // Constructor for an error result
    Result(const Error &errorInfo)
        : success_(false), value_(), errorInfo_(errorInfo) {}

    // Check if the result is an error
    bool hasValue() const { return !success_; }

    // Retrieve the value (only call if not an error)
    const T &value() const { return value_; }

    // Retrieve the error (only call if isError() returns true)
    const Error &error() const { return errorInfo_; }

private:
    bool success_;
    T value_;
    Error errorInfo_;
};

// Specialization for Result<void>I
template <> class Result<void> {
public:
    // Constructor for an error result
    Result(const Error &errorInfo) : errorInfo_(errorInfo) {}

    // Check if the result is an error
    bool isError() const { return errorInfo_ ? true : false; }

    // There is no value for Result<void>, so no value() method

    // Retrieve the error
    const Error &error() const { return errorInfo_; }

private:
    Error errorInfo_;
};