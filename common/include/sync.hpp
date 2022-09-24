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

#include <common.hpp>

enum class LockGuardSignalizeOption { Signalize, NotSignalize };

template <typename Lock> class LockGuard {
public:
    PREVENT_COPY_AND_MOVE(LockGuard)

    inline LockGuard(Lock &lock, LockGuardSignalizeOption option = LockGuardSignalizeOption::NotSignalize)
        : lock_(lock), signalizeOption_(option) {
        lock_.get();
    }
    inline ~LockGuard() {
        lock_.release();
        if (signalizeOption_ == LockGuardSignalizeOption::Signalize)
            lock_.signalize();
    }

private:
    Lock& lock_;
    LockGuardSignalizeOption signalizeOption_;
};

class BaseLock {
public:
    BaseLock() = default;

    ~BaseLock() = default;

    PREVENT_COPY_AND_MOVE(BaseLock)

    virtual void get() = 0;

    virtual void release() = 0;

    virtual void signalize() = 0;
};

class DummyLock : public BaseLock {
public:
    DummyLock() : BaseLock(){};

    inline void get() override {}

    inline void release() override {}

    inline void signalize() override {}
};