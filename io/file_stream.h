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

#ifndef IO_FILE_STREAM_H_
#define IO_FILE_STREAM_H_

#include "input_stream.h"
#include "output_stream.h"
#include <string>
#include <utility>

namespace programmerjake
{
namespace voxels
{
namespace io
{
class FileInputStream final : public InputStream
{
private:
    struct Implementation;

private:
    Implementation *implementation;

public:
    explicit FileInputStream(std::string fileName);
    virtual ~FileInputStream();
    FileInputStream(FileInputStream &&rt) noexcept : InputStream(std::move(rt)),
                                                     implementation(rt.implementation)
    {
        rt.implementation = nullptr;
    }
    FileInputStream &operator=(FileInputStream rt) noexcept
    {
        InputStream::operator=(std::move(rt));
        std::swap(implementation, rt.implementation);
        return *this;
    }
    virtual ReadBytesResult readBytes(
        unsigned char *buffer,
        std::size_t bufferSize,
        const std::chrono::steady_clock::time_point *timeout) override;
    void close();
};

class FileOutputStream final : public OutputStream
{
private:
    struct Implementation;

private:
    Implementation *implementation;

public:
    explicit FileOutputStream(std::string fileName);
    virtual ~FileOutputStream();
    FileOutputStream(FileOutputStream &&rt) noexcept : OutputStream(std::move(rt)),
                                                       implementation(rt.implementation)
    {
        rt.implementation = nullptr;
    }
    FileOutputStream &operator=(FileOutputStream rt) noexcept
    {
        OutputStream::operator=(std::move(rt));
        std::swap(implementation, rt.implementation);
        return *this;
    }
    virtual void writeBytes(const unsigned char *buffer, std::size_t bufferSize) override;
    virtual void flush() override;
    void close();
};
}
}
}

#endif /* IO_FILE_STREAM_H_ */
