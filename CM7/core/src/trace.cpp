/**
 ******************************************************************************
 * @file           trace.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          class to handle and send trace packets
 ******************************************************************************
 * @attention
 * Copyright (c) 2024 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <lwip/api.h>

#include <carbon/trace.hpp>

namespace CARBON {

#ifdef FREERTOS_USE_TRACE
Trace::~Trace() {}

bool Trace::init() { return true; }

void Trace::sendTrace() {}

static Trace traceSingleton;

Trace &Trace::instance() { return traceSingleton; }
#endif

} // namespace CARBON
