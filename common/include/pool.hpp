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

#include <cstring>
#include <new>

class MemoryRegion {
public:
    MemoryRegion(uint32_t startAddress, uint32_t size) : size_(size) {
        data_ = new (reinterpret_cast<void *>(startAddress)) uint8_t[size_];
    }

    ~MemoryRegion() = default;

    PREVENT_COPY_AND_MOVE(MemoryRegion);

    uint32_t getSize() const { return size_; }

    uint8_t *getAddress() const { return data_; }

private:
    uint8_t *data_{nullptr};
    uint32_t size_;
    bool isInitialized{false};
};

template <size_t blockSize, uint32_t alignment> class MemoryAllocatorRaw {
public:
    MemoryAllocatorRaw(uint32_t startAddress, uint32_t size)
        : memoryRegion_(startAddress, size) {}

    ~MemoryAllocatorRaw() = default;

    PREVENT_COPY_AND_MOVE(MemoryAllocatorRaw)

    void init() {
        sizeTotalBytes_ = memoryRegion_.getSize();
        startAddress_ = memoryRegion_.getAddress();
        alignedBlockSize_ = alignBlockSize(blockSize, alignment);
        firstAlignedAddress_ = alignAddress(startAddress_, alignment);
        alignedAddress_ = firstAlignedAddress_;
    }

    bool getBlock(uint8_t **block) {
        if (alignedAddress_ >= startAddress_ + sizeTotalBytes_) {
            *block = nullptr;
            return false;
        }
        *block = alignAddress(alignedAddress_, alignment);
        alignedAddress_ = *block + alignedBlockSize_;
        return true;
    }

    bool getBlock(uint8_t **block, uint32_t index) {
        *block = firstAlignedAddress_ + index * alignedBlockSize_;
        if (*block > startAddress_ + sizeTotalBytes_) {
            *block = nullptr;
            return false;
        }
        return true;
    }

    bool blockBelongs(const void *block) const {
        uint8_t *blockPtr = reinterpret_cast<uint8_t *>(block);
        bool check = true;
        check = check && (blockPtr >= startAddress_);
        check = check && (blockPtr < (firstAlignedAddress_ + sizeTotalBytes_));
        check = check && (blockPtr == alignAddress(blockPtr, alignment));
        return check;
    }

    void reset() { alignedAddress_ = firstAlignedAddress_; }

    const MemoryRegion &getMemoryRegion() { return memoryRegion_; }

    uint32_t getTotalSize() const { return sizeTotalBytes_; }

    uint32_t getAlignedBlockSize() const { return alignedBlockSize_; }

    uint32_t getStartAddress() const { return startAddress_; }

    uint32_t getAlignedStartAddress() const { return firstAlignedAddress_; }

    uint32_t getNumberOfAlignedElements() const {
        return getNumberOfElements(startAddress_, sizeTotalBytes_, blockSize,
                                   alignment);
    }

    static constexpr uint32_t predictAlignedBlockSize(uint32_t objectSize,
                                                      uint32_t memAlignment) {
        return alignBlockSize(objectSize, memAlignment);
    }

    static constexpr uint32_t getNumberOfElements(uint8_t *address,
                                                  uint32_t bufferSize,
                                                  uint32_t objectSize,
                                                  uint32_t memAlignment) {
        RAW_DIAG(
            "address %lu, bufferSize %lu, objectSize %lu, memAlignment %lu",
            reinterpret_cast<uint32_t>(address), bufferSize, objectSize,
            memAlignment);
        uint8_t *startAddr = alignAddress(address, memAlignment);
        uint32_t objectSizeAligned =
            predictAlignedBlockSize(objectSize, memAlignment);
        uint8_t *endAddr = (address + bufferSize) > startAddr
                               ? (address + bufferSize)
                               : startAddr;
        uint32_t actualSize = reinterpret_cast<uint32_t>(endAddr) -
                              reinterpret_cast<uint32_t>(startAddr);
        RAW_DIAG(
            "startAddr %lu, objectSizeAligned %lu, endAddr %lu, actualSize %lu",
            reinterpret_cast<uint32_t>(startAddr), objectSizeAligned,
            reinterpret_cast<uint32_t>(endAddr), actualSize);
        return (actualSize / objectSizeAligned);
    }

private:
    MemoryRegion memoryRegion_;
    uint32_t sizeTotalBytes_{0};
    uint32_t alignedBlockSize_{0};
    uint8_t *startAddress_{nullptr};
    uint8_t *firstAlignedAddress_{nullptr};
    uint8_t *alignedAddress_{nullptr};

    static constexpr uint32_t alignBlockSize(uint32_t value,
                                             uint32_t memAlignment) {
        return (((value + static_cast<uint32_t>(memAlignment) - 1)) &
                (~(static_cast<uint32_t>(memAlignment) - 1)));
    }

    static constexpr uint8_t *alignAddress(uint8_t *address,
                                           uint32_t memAlignment) {
        return (reinterpret_cast<uint8_t *>(
            reinterpret_cast<uint32_t>(address + (memAlignment - 1)) &
            (~(memAlignment - 1))));
    }
};

class StackBase {
public:
    ~StackBase() = default;

    PREVENT_COPY_AND_MOVE(StackBase);

    bool isEmpty() { return (this->top_ == 0); }

    bool isFull() { return (this->top_ == size_); }

protected:
    StackBase(uint32_t size) : size_(size), top_(0) {}

    void overFlow() {}
    void underFlow() {}

    uint32_t size_;
    uint32_t top_;
};

template <typename ObjectType, typename Lock, uint32_t NElements>
class Stack : public StackBase {
public:
    Stack() : StackBase(NElements) {}

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
    ObjectType *data[NElements + 1];
};

template <typename MemoryAllocatorRaw, typename LockType, uint32_t NElements>
class MemoryPoolRaw {
public:
    MemoryPoolRaw(MemoryAllocatorRaw &memoryAllocator)
        : memoryAllocator_(memoryAllocator) {
        memoryAllocator_.init();
        if (memoryAllocator_.getNumberOfAlignedElements() < NElements + 1) {
            RAW_DIAG("memomy buffer too small");
        }
    }

    ~MemoryPoolRaw() { memoryAllocator_.reset(); }

    PREVENT_COPY_AND_MOVE(MemoryPoolRaw)

    bool init() {
        LockType lock;
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

    inline bool allocate(uint8_t **block) {
        LockType lock;
        bool res = pool_.pop(block);
        if (!res) {
            RAW_DIAG("bad allocation");
            return false;
        }
        return true;
    }
    /*TODO avoid freeing twice the same block*/
    inline bool free(uint8_t *block) {
        LockType lock;
        bool res = memoryAllocator_.blockBelongs(block);
        if (!res) {
            RAW_DIAG("block does not belong to pool");
            return false;
        }
        res = pool_.push(block);
        if (!res) {
            RAW_DIAG("error while freeing block");
            return false;
        }
        return true;
    }

protected:
    volatile bool isInitislized_{false};
    uint32_t size_{0};
    MemoryAllocatorRaw &memoryAllocator_;
    Stack<uint8_t, Lock<DummyMutex>, NElements> pool_;
};

template <typename ObjectType, uint32_t aligment> class Buffer {
public:
    Buffer(uint32_t startAddress, uint32_t size)
        : memoryAllocatorRaw_(startAddress, size) {}

    ~Buffer() = default;

    PREVENT_COPY_AND_MOVE(Buffer)

    void init() { memoryAllocatorRaw_.init(); }

    inline bool insert(const ObjectType &object, const uint32_t index) {
        uint8_t *data;
        if (!(memoryAllocatorRaw_.getBlock(&data, index))) {
            return false;
        }
        std::memcpy(reinterpret_cast<void *>(data),
                    reinterpret_cast<const void *>(&object), sizeof(object));
#ifdef TEST_FIFO
        RAW_DIAG("inserted value at %lu, from the address %lu, length %lu",
                 reinterpret_cast<uint32_t>(data),
                 reinterpret_cast<uint32_t>(&object), sizeof(object));
#endif
        return true;
    }

    inline bool remove(ObjectType &object, const uint32_t index) {
        uint8_t *data;
        if (!(memoryAllocatorRaw_.getBlock(&data, index))) {
            return false;
        }
        std::memcpy(reinterpret_cast<void *>(&object),
                    reinterpret_cast<const void *>(data), sizeof(object));
#ifdef TEST_FIFO
        RAW_DIAG("removed value at %lu, in the address %lu, length %lu",
                 reinterpret_cast<uint32_t>(data),
                 reinterpret_cast<uint32_t>(&object), sizeof(object));
#endif
        return true;
    }

    uint32_t getNumberOfAlignedElements() const {
        return memoryAllocatorRaw_.getNumberOfAlignedElements();
    }

    typedef ObjectType Type;

private:
    MemoryAllocatorRaw<sizeof(ObjectType), aligment> memoryAllocatorRaw_;
};