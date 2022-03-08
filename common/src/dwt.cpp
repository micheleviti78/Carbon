/**
 ******************************************************************************
 * @file           dwt.cpp
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

#include <dwt.hpp>

DataWatchpointTraceUnit::DataWatchpointTraceUnit() : dwt(DWT) {}

void DataWatchpointTraceUnit::init() {
  SET_BIT(dwt->CTRL, DWT_CTRL_CYCCNTENA_Msk);
}

uint32_t DataWatchpointTraceUnit::getClkCycles() {
  return READ_REG(dwt->CYCCNT);
}