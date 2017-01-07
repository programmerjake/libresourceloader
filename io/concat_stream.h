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

#ifndef IO_CONCAT_STREAM_H_
#define IO_CONCAT_STREAM_H_

#include "input_stream.h"
#include <vector>
#include <initializer_list>
#include "../util/constexpr_assert.h"

namespace programmerjake
{
namespace voxels
{
namespace io
{
class ConcatInputStream final : public InputStream
{
private:
    std::vector<std::shared_ptr<InputStream>> streams;
    std::size_t streamIndex;

public:
    ConcatInputStream(std::initializer_list<std::shared_ptr<InputStream>> streams)
        : streams(streams), streamIndex(0)
    {
    }
    ConcatInputStream(std::vector<std::shared_ptr<InputStream>> streams)
        : streams(std::move(streams)), streamIndex(0)
    {
    }
    virtual ReadBytesResult readBytes(unsigned char *buffer,
                                      std::size_t bufferSize,
                                      const std::chrono::steady_clock::time_point *timeout) override
    {
        std::size_t totalReadCount = 0;
        while(bufferSize > 0)
        {
            if(streamIndex >= streams.size())
                return ReadBytesResult(totalReadCount, true);
            auto result = streams[streamIndex]->readBytes(buffer, bufferSize, timeout);
            constexprAssert(result.readCount <= bufferSize);
            totalReadCount += result.readCount;
            buffer += result.readCount;
            bufferSize -= result.readCount;
            if(result.hitEOF)
                streamIndex++;
            else if(timeout)
                break;
        }
        return ReadBytesResult(totalReadCount, streamIndex >= streams.size());
    }
};
}
}
}

#endif /* IO_CONCAT_STREAM_H_ */
