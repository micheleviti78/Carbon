/**
 ******************************************************************************
 * @file           dwt.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 Data Watchpoint and Trace Unit, header file
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

#include <stm32h7xx_hal.h>

class DataWatchpointTraceUnit {
public:
  DataWatchpointTraceUnit();
  ~DataWatchpointTraceUnit() = default;
  DataWatchpointTraceUnit(const DataWatchpointTraceUnit &dwt) = delete;
  DataWatchpointTraceUnit(DataWatchpointTraceUnit &&dwt) = delete;
  DataWatchpointTraceUnit &
  operator=(const DataWatchpointTraceUnit &dwt) = delete;
  DataWatchpointTraceUnit &operator=(DataWatchpointTraceUnit &&dwt) = delete;
  void init();
  uint32_t getClkCycles();

private:
  DWT_Type *dwt;
};