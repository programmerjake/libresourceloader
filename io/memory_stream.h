/*
 * Copyright (C) 2012-2016 Jacob R. Lifshay
 * This file is part of Voxels.
 *
 * Voxels is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Voxels is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Voxels; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifndef IO_MEMORY_STREAM_H_
#define IO_MEMORY_STREAM_H_

#include "input_stream.h"
#include "output_stream.h"
#include <memory>
#include <vector>

namespace programmerjake
{
namespace voxels
{
namespace io
{
class MemoryInputStream final : public InputStream
{
private:
    std::shared_ptr<const unsigned char> memoryBuffer;
    std::size_t memoryBufferSize;
    std::size_t position;

public:
    explicit MemoryInputStream(std::shared_ptr<const unsigned char> memoryBuffer,
                               std::size_t memoryBufferSize)
        : memoryBuffer(std::move(memoryBuffer)), memoryBufferSize(memoryBufferSize), position(0)
    {
    }
    explicit MemoryInputStream(std::shared_ptr<const std::vector<unsigned char>> memoryBuffer)
        : MemoryInputStream(
              std::shared_ptr<const unsigned char>(memoryBuffer, memoryBuffer->data()),
              memoryBuffer->size())
    {
    }
    explicit MemoryInputStream(std::vector<unsigned char> memoryBuffer)
        : MemoryInputStream(std::make_shared<std::vector<unsigned char>>(std::move(memoryBuffer)))
    {
    }
    explicit MemoryInputStream(const unsigned char *memoryBuffer, std::size_t memoryBufferSize)
        : MemoryInputStream(std::make_shared<std::vector<unsigned char>>(
              memoryBuffer, memoryBuffer + memoryBufferSize))
    {
    }
    virtual ReadBytesResult readBytes(unsigned char *buffer,
                                      std::size_t bufferSize,
                                      const std::chrono::steady_clock::time_point *timeout) override
    {
        if(bufferSize > memoryBufferSize - position)
            bufferSize = memoryBufferSize - position;
        for(std::size_t i = 0; i < bufferSize; i++)
        {
            buffer[i] = memoryBuffer.get()[position++];
        }
        return ReadBytesResult(bufferSize, position >= memoryBufferSize);
    }
};

class MemoryOutputStream final : public OutputStream
{
private:
    std::vector<unsigned char> outputBuffer;

public:
    MemoryOutputStream() : outputBuffer()
    {
    }
    explicit MemoryOutputStream(std::size_t reservedSize) : outputBuffer()
    {
        outputBuffer.reserve(reservedSize);
    }
    virtual void writeBytes(const unsigned char *buffer, std::size_t bufferSize) override
    {
        outputBuffer.insert(outputBuffer.end(), buffer, buffer + bufferSize);
    }
    virtual void flush() override
    {
    }
    std::vector<unsigned char> releaseBuffer()
    {
        std::vector<unsigned char> retval;
        retval.swap(outputBuffer);
        return retval;
    }
};
}
}
}

#endif /* IO_MEMORY_STREAM_H_ */
