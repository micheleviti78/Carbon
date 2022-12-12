/**
 ******************************************************************************
 * @file           irq.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Sep. 2022
 * @brief          DISCO-STM32H747 basic IRQ interface
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

#include <carbon/irq.hpp>

namespace CARBON {

IRQ::LockStatus IRQ::irqLockStartStatus = IRQ::LockStatus::Unlocked;
uint32_t IRQ::irqLockCounter = uint32_t{0};

} // namespace CARBON
