/**
 ******************************************************************************
 * @file           sync.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 sync utilities
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

#include <carbon/common.hpp>

template <typename Lock> class LockGuard {
public:
    PREVENT_COPY_AND_MOVE(LockGuard)

    inline LockGuard(Lock &lock) : lock_(lock) { lock_.get(); }

    inline ~LockGuard() { lock_.release(); }

private:
    Lock &lock_;
};

class BaseLock {
public:
    BaseLock() = default;

    virtual ~BaseLock() = default;

    PREVENT_COPY_AND_MOVE(BaseLock)

    virtual void get() = 0;

    virtual void release() = 0;
};

class DummyLock : public BaseLock {
public:
    DummyLock() : BaseLock(){};

    ~DummyLock() override = default;

    inline void get() override {}

    inline void release() override {}
};
