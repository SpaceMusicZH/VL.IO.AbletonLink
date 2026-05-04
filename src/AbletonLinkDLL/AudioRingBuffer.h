// Single-producer / single-consumer lock-free ring buffer for interleaved
// float audio samples. Power-of-two capacity, atomic indices, no allocations
// after construction.
//
// Modeled on Void-LinkAudio's AudioRingBuffer
// (https://github.com/gluon/Void-LinkAudio/blob/main/core/AudioRingBuffer.h),
// SPDX-License-Identifier: GPL-2.0-or-later, (C) 2026 Julien Bayle.

#pragma once

#include <atomic>
#include <cstddef>
#include <cstring>
#include <vector>

class AudioRingBuffer
{
public:
    explicit AudioRingBuffer(std::size_t requestedCapacity = 32768)
    {
        std::size_t cap = 1;
        while (cap < requestedCapacity) cap <<= 1;
        mBuffer.assign(cap, 0.0f);
        mMask = cap - 1;
        mCapacity = cap;
    }

    std::size_t capacity() const noexcept { return mCapacity; }

    std::size_t write(const float* src, std::size_t count) noexcept
    {
        const std::size_t w = mWriteIdx.load(std::memory_order_relaxed);
        const std::size_t r = mReadIdx.load(std::memory_order_acquire);

        const std::size_t free = mCapacity - (w - r);
        const std::size_t toWrite = (count < free) ? count : free;

        for (std::size_t i = 0; i < toWrite; ++i)
            mBuffer[(w + i) & mMask] = src[i];

        mWriteIdx.store(w + toWrite, std::memory_order_release);
        return toWrite;
    }

    std::size_t read(float* dst, std::size_t count) noexcept
    {
        const std::size_t r = mReadIdx.load(std::memory_order_relaxed);
        const std::size_t w = mWriteIdx.load(std::memory_order_acquire);

        const std::size_t avail = w - r;
        const std::size_t toRead = (count < avail) ? count : avail;

        for (std::size_t i = 0; i < toRead; ++i)
            dst[i] = mBuffer[(r + i) & mMask];

        mReadIdx.store(r + toRead, std::memory_order_release);
        return toRead;
    }

    std::size_t available() const noexcept
    {
        const std::size_t r = mReadIdx.load(std::memory_order_relaxed);
        const std::size_t w = mWriteIdx.load(std::memory_order_acquire);
        return w - r;
    }

    std::size_t writeAvailable() const noexcept
    {
        const std::size_t w = mWriteIdx.load(std::memory_order_relaxed);
        const std::size_t r = mReadIdx.load(std::memory_order_acquire);
        return mCapacity - (w - r);
    }

    void reset() noexcept
    {
        mReadIdx.store(0, std::memory_order_relaxed);
        mWriteIdx.store(0, std::memory_order_relaxed);
    }

private:
    std::vector<float> mBuffer;
    std::size_t        mCapacity = 0;
    std::size_t        mMask = 0;
    std::atomic<std::size_t> mReadIdx{0};
    std::atomic<std::size_t> mWriteIdx{0};
};
