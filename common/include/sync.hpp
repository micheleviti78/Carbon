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

template <typename Mutex> class Lock {
public:
    Lock() = default;

    ~Lock() = default;

    PREVENT_COPY_AND_MOVE(Lock)

    enum SignalizeOption { signalize, notSignalize };

    inline Lock(SignalizeOption option = SignalizeOption::notSignalize)
        : signalizeOption_(option) {
        mutex_.get();
    }
    inline ~Lock() {
        mutex_.release();
        if (signalizeOption_ == SignalizeOption::signalize)
            mutex_.signalize();
    }

private:
    SignalizeOption signalizeOption_;
    Mutex mutex_;
};

class BaseMutex {
public:
    BaseMutex() = default;

    ~BaseMutex() = default;

    PREVENT_COPY_AND_MOVE(BaseMutex)

    void get() = 0;

    void release() = 0;

    void signalize() = 0;
}

class DummyMutex()
    : public BaseMutex {
public:
    DummyMutex() = default;

    inline void get() {}

    inline void release() {}

    inline void signalize() {}
}

Lock<DummyMutex> DummyLock;