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

#ifndef IO_OUTPUT_STREAM_H_
#define IO_OUTPUT_STREAM_H_

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
struct OutputStream : virtual public StreamBase
{
    virtual void writeBytes(const unsigned char *buffer, std::size_t bufferSize) = 0;
    virtual void flush() = 0;
    void writeByte(unsigned char byte)
    {
        writeBytes(&byte, 1);
    }
    void writeBool(bool value)
    {
        writeByte(value ? 1 : 0);
    }
    static_assert(std::is_same<std::uint8_t, unsigned char>::value, "");
    void writeU8(std::uint8_t value)
    {
        writeByte(value);
    }
    void writeS8(std::int8_t value)
    {
        writeU8(value);
    }
    void writeU16(std::uint16_t value)
    {
        const std::size_t byteCount = 2;
        std::uint8_t bytes[byteCount] = {
            static_cast<std::uint8_t>(value), static_cast<std::uint8_t>(value >> 8),
        };
        writeBytes(bytes, byteCount);
    }
    void writeS16(std::int16_t value)
    {
        writeU16(value);
    }
    void writeU32(std::uint32_t value)
    {
        const std::size_t byteCount = 4;
        std::uint8_t bytes[byteCount] = {
            static_cast<std::uint8_t>(value),
            static_cast<std::uint8_t>(value >> 8),
            static_cast<std::uint8_t>(value >> 16),
            static_cast<std::uint8_t>(value >> 24),
        };
        writeBytes(bytes, byteCount);
    }
    void writeS32(std::int32_t value)
    {
        writeU32(value);
    }
    void writeU64(std::uint64_t value)
    {
        const std::size_t byteCount = 8;
        std::uint8_t bytes[byteCount] = {
            static_cast<std::uint8_t>(value),
            static_cast<std::uint8_t>(value >> 8),
            static_cast<std::uint8_t>(value >> 16),
            static_cast<std::uint8_t>(value >> 24),
            static_cast<std::uint8_t>(value >> 32),
            static_cast<std::uint8_t>(value >> 40),
            static_cast<std::uint8_t>(value >> 48),
            static_cast<std::uint8_t>(value >> 56),
        };
        writeBytes(bytes, byteCount);
    }
    void writeS64(std::int64_t value)
    {
        writeU64(value);
    }
    void writeF32(float value)
    {
        static_assert(
            std::numeric_limits<float>::is_iec559 && sizeof(float) == sizeof(std::uint32_t), "");
        union
        {
            float f;
            std::uint32_t i;
        } u;
        u.f = value;
        writeU32(u.i);
    }
    void writeF64(double value)
    {
        static_assert(
            std::numeric_limits<double>::is_iec559 && sizeof(double) == sizeof(std::uint64_t), "");
        union
        {
            double f;
            std::uint64_t i;
        } u;
        u.f = value;
        writeU64(u.i);
    }
};
}
}
}

#endif /* IO_OUTPUT_STREAM_H_ */
