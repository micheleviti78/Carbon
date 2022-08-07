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

template <typename Mutex> class Lock {
public:
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
