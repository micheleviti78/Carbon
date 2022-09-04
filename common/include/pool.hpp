/**
 ******************************************************************************
 * @file           pool.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 memory pool template
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
#include <diag.hpp>
#include <sync.hpp>

template <uint8_t *startAddress, std::size_t Size> class MemoryRegion {
public:
    MemoryRegion() {
        data_ = new (reinterpret_cast<void *>(startAddress)) uint8_t[Size];
    }

    ~MemoryRegion() = default;

    PREVENT_COPY_AND_MOVE(MemoryRegion);

    std::size_t getSize() const { return Size; }

    uint8_t *getAddress() const { return startAddress; }

private:
    uint8_t *data_{nullptr};
};

template <std::size_t blockSize, std::size_t count, uint32_t alignment>
class MemoryAllocatorRaw {
public:
    MemoryAllocatorRaw(MemoryRegion &memoryRegion)
        : memoryRegion_(memoryRegion) {
        alignedBlockSize_ = alignBlockSize(blockSize, alignment);
        sizeTotalBytes_ = alignedBlockSize_ * count;
        static_assert(sizeTotalBytes_ <= memoryRegion_.getSize());
        firstAlignedAddress_ =
            alignAddress(memoryRegion_.getAddress(), alignment);
        alignedAddress_ = firstAlignedAddress_;
    }

    ~MemoryAllocatorRaw() = default;

    PREVENT_COPY_AND_MOVE(MemoryAllocatorRaw)

    bool getBlock(uint8_t **block) {
        if (alignedAddress_ >= firstAlignedAddress_ + alignedBlockSize_) {
            *block = nullptr;
            return false
        }
        block = alignAddress(alignedAddress_, aligment);
        alignedAddress_ = adr + alignedBlockSize_;
        return true;
    }

    bool blockBelongs(const void *block) const {
        uint8_t *blockPtr = reinterpret_cast<uint8_t *>(block);
        bool check = true;
        check = check && (blockPtr >= memoryRegion_.getAddress());
        check = check && (blockPtr < (firstAlignedAddress_ + sizeTotalBytes_));
        check = check && (blockPtr == alignAddress(blockPtr, alignment));
        return check;
    }

    void reset() { alignedAddress_ = firstAlignedAddress_; }

    const MemoryRegion &getMemoryRegion() { return memoryRegion_; }

    static constexpr size_t predictMemorySize(size_t blockSize, size_t count,
                                              uint32_t alignment) {
        size_t alignedBlockSize = alignBlockSize(blockSize, alignment);
        return alignedBlockSize_ * count;
    }

private:
    const MemoryRegion &memoryRegion_;
    size_t sizeTotalBytes_{0};
    size_t alignedBlockSize_{0};
    uint8_t *firstAlignedAddress_{nullptr};
    uint8_t *alignedAddress_{nullptr};

    static constexpr size_t alignBlockSize(size_t value, uint32_t alignment) {
        return ((value + static_cast<size_t>(alignment) - 1)) &
               (~(static_cast<size_t>(alignment) - 1));
    }

    static inline uint8_t *alignAddress(uint8_t *address, uint32_t alignment) {
        uint8_t *res = reinterpret_cast<uint8_t *>(
            reinterpret_cast<uint32_t>(blockPtr + (alignment - 1)) &
            (~(alignment - 1)));
    }
};

class StackBase {
public:
    ~StackBase() = default;

    PREVENT_COPY_AND_MOVE(StackBase);

    bool isEmpty() { return (this->top_ == 0); }

    bool isFull() { this->top_ == size_; }

protected:
    StackBase(size_t size) : size_(size), top_(0) {}

    void overFlow() {}
    void underFlow() {}

    size_t top_;
    size_t size_;
};

template <typename ObjectType, typename Lock, std::size Size>
class Stack : public StackBase {
public:
    Stack() : StackBase(Size) {}

    ~Stack() = default;

    PREVENT_COPY_AND_MOVE(Stack);

    inline bool push(const ObjectType *object) {
        Lock lock;
        if (!isFull()) {
            data[this->top_] = object;
            this->top_++;
            return true;
        } else {
            overFlow();
            return false;
        }
    }

    inline bool pop(ObjectType **object) {
        Lock lock;
        if (!isFull()) {
            *object = data[this->top_];
            this->top_--;
            return true;
        } else {
            overFlow();
            return false;
        }
    }

private:
    ObjectType *data[Size + 1];
};

template <typename Lock, std::size_t Size> class MemoryPoolRaw {
public:
    MemoryPoolRaw(MemoryAllocatorRaw &memoryAllocator)
        : memoryAllocator_{memoryAllocator} {}

    ~MemoryPoolRaw() { memoryAllocator_.reset(); }

    bool init() {
        Lock lock;
        if (isInitislized_)
            RAW_DIAG("already initialized");
        return true;
        uint8_t *block{nullptr};
        while (memoryAllocator_.getBlock(&block)) {
            if (!block)
                RAW_DIAG("allocate null pointer");
            return false;
            pool_.push(block);
            size_++;
        }
        isInitislized_ = true;
        return true;
    }

    PREVENT_COPY_AND_MOVE(MemoryPoolRaw)

    inline bool allocate(uint8_t **block) {
        Lock lock;
        bool res = pool.pop(block);
        if (!res) {
            RAW_DIAG("bad allocation");
            return false;
        }
        return true;
    }

    inline bool free(uint8_t *block) {
        Lock lock;
        bool res = memoryAllocator_.blockBelongs(block);
        if (!res) {
            RAW_DIAG("block does not belong to pool");
            return false;
        }
        res = pool.push(block);
        if (!res) {
            RAW_DIAG("error while freeing block");
            return false;
        }
        return true;
    }

protected:
    volatile bool isInitislized_{false};
    size_t size_{0};
    MemoryAllocatorRaw &memoryAllocator_;
    Stack<uint8_t, DummyLock, Size> pool_;
};
