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

#ifndef IO_INPUT_STREAM_H_
#define IO_INPUT_STREAM_H_

#include "stream_base.h"
#include <cstdint>
#include <type_traits>
#include <chrono>
#include <limits>

namespace programmerjake
{
namespace voxels
{
namespace io
{
struct InputStream : virtual public StreamBase
{
    struct ReadBytesResult
    {
        std::size_t readCount;
        bool hitEOF;
        constexpr ReadBytesResult(std::size_t readCount, bool hitEOF)
            : readCount(readCount), hitEOF(hitEOF)
        {
        }
    };
    virtual ReadBytesResult readBytes(unsigned char *buffer,
                                      std::size_t bufferSize,
                                      const std::chrono::steady_clock::time_point *timeout) = 0;
    ReadBytesResult readBytes(unsigned char *buffer, std::size_t bufferSize)
    {
        return readBytes(buffer, bufferSize, nullptr);
    }
    ReadBytesResult readBytes(unsigned char *buffer,
                              std::size_t bufferSize,
                              const std::chrono::steady_clock::time_point &timeout)
    {
        return readBytes(buffer, bufferSize, &timeout);
    }
    ReadBytesResult readAvailableBytes(unsigned char *buffer, std::size_t bufferSize)
    {
        return readBytes(buffer, bufferSize, std::chrono::steady_clock::time_point::min());
    }
    std::size_t readAllBytes(unsigned char *buffer,
                             std::size_t bufferSize,
                             bool throwOnEarlyEOF = true)
    {
        std::size_t retval = 0;
        while(bufferSize != 0)
        {
            auto result = readBytes(buffer, bufferSize);
            retval += result.readCount;
            bufferSize -= result.readCount;
            buffer += result.readCount;
            if(result.hitEOF)
                break;
        }
        if(bufferSize != 0 && throwOnEarlyEOF)
            throw EOFError();
        return retval;
    }
    unsigned char readByte()
    {
        unsigned char retval;
        readAllBytes(&retval, 1);
        return retval;
    }
    bool readBool()
    {
        return readByte() != 0;
    }
    static_assert(std::is_same<std::uint8_t, unsigned char>::value, "");
    std::uint8_t readU8()
    {
        return readByte();
    }
    std::int8_t readS8()
    {
        return readU8();
    }
    std::uint16_t readU16()
    {
        const std::size_t byteCount = 2;
        std::uint8_t bytes[byteCount];
        readAllBytes(bytes, byteCount);
        return (static_cast<std::uint16_t>(bytes[1]) << 8) | bytes[0];
    }
    std::int16_t readS16()
    {
        return readU16();
    }
    std::uint32_t readU32()
    {
        const std::size_t byteCount = 4;
        std::uint8_t bytes[byteCount];
        readAllBytes(bytes, byteCount);
        return (static_cast<std::uint32_t>(bytes[3]) << 24)
               | (static_cast<std::uint32_t>(bytes[2]) << 16)
               | (static_cast<std::uint32_t>(bytes[1]) << 8) | bytes[0];
    }
    std::int32_t readS32()
    {
        return readU32();
    }
    std::uint64_t readU64()
    {
        const std::size_t byteCount = 8;
        std::uint8_t bytes[byteCount];
        readAllBytes(bytes, byteCount);
        return (static_cast<std::uint64_t>(bytes[7]) << 56)
               | (static_cast<std::uint64_t>(bytes[6]) << 48)
               | (static_cast<std::uint64_t>(bytes[5]) << 40)
               | (static_cast<std::uint64_t>(bytes[4]) << 32)
               | (static_cast<std::uint64_t>(bytes[3]) << 24)
               | (static_cast<std::uint64_t>(bytes[2]) << 16)
               | (static_cast<std::uint64_t>(bytes[1]) << 8) | bytes[0];
    }
    std::int64_t readS64()
    {
        return readU64();
    }
    float readF32()
    {
        static_assert(
            std::numeric_limits<float>::is_iec559 && sizeof(float) == sizeof(std::uint32_t), "");
        union
        {
            float f;
            std::uint32_t i;
        } u;
        u.i = readU32();
        return u.f;
    }
    double readF64()
    {
        static_assert(
            std::numeric_limits<double>::is_iec559 && sizeof(double) == sizeof(std::uint64_t), "");
        union
        {
            double f;
            std::uint64_t i;
        } u;
        u.i = readU64();
        return u.f;
    }
};
}
}
}

#endif /* IO_INPUT_STREAM_H_ */
