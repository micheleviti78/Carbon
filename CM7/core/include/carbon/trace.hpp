/**
 ******************************************************************************
 * @file           trace.hpp
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

#pragma once

#ifdef FREERTOS_USE_TRACE
#include <carbon/common.hpp>

namespace CARBON {

class Trace {
public:
    Trace() = default;

    PREVENT_COPY_AND_MOVE(Trace)

    ~Trace();

    bool init();

    void sendTrace();

    static Trace &instance();
};

} // namespace CARBON
#endif
