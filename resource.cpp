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
#include "io/memory_stream.h"
#include <zip.h>
#include <iostream>
#include <cstdlib>
#include "resource.h"

extern "C" {
extern const unsigned char _binary_res_zip_start;
extern const unsigned char _binary_res_zip_end;
}

namespace programmerjake
{
namespace voxels
{
namespace resource
{
struct ResourceManager::Implementation final : public io::InputStream
{
    Implementation(const Implementation &) = delete;
    Implementation &operator=(const Implementation &) = delete;

private:
    struct ZipDeleter final
    {
        void operator()(zip_t *v) const noexcept
        {
            zip_close(v);
        }
    };
    struct ZipFileDeleter final
    {
        void operator()(zip_file_t *v) const noexcept
        {
            zip_fclose(v);
        }
    };

private:
    zip_t *zip = nullptr;
    zip_file_t *zipFile = nullptr;
    unsigned char nextByte = 0;
    bool hasNextByte = false;
    bool hitEndOfFile = false;

public:
    explicit Implementation(const std::string &name)
    {
        zip_error_t zipError;
        zip_error_init(&zipError);
        auto source = zip_source_buffer_create(static_cast<const void *>(&_binary_res_zip_start),
                                               &_binary_res_zip_end - &_binary_res_zip_start,
                                               false,
                                               &zipError);
        if(!source)
        {
            std::cerr << "libzip error: zip_source_buffer_create: " << zip_error_strerror(&zipError)
                      << std::endl;
            zip_error_fini(&zipError);
            abort();
        }
        zip = zip_open_from_source(source, ZIP_RDONLY, &zipError);
        if(!zip)
        {
            std::cerr << "libzip error: zip_open_from_source: " << zip_error_strerror(&zipError)
                      << std::endl;
            zip_source_free(source);
            zip_error_fini(&zipError);
            abort();
        }
        zip_error_fini(&zipError);
        zipFile = zip_fopen(zip, name.c_str(), ZIP_FL_ENC_STRICT);
        if(!zipFile)
        {
            if(zip_error_code_zip(zip_get_error(zip)) == ZIP_ER_NOENT)
            {
                zip_close(zip);
                throw io::IOError(std::make_error_code(std::errc::no_such_file_or_directory), "file not found: " + name);
            }
            std::cerr << "libzip error: zip_fopen: " << zip_error_strerror(zip_get_error(zip))
                      << std::endl;
            zip_close(zip);
            abort();
        }
    }
    virtual ~Implementation()
    {
        zip_fclose(zipFile);
        zip_close(zip);
    }
    virtual ReadBytesResult readBytes(unsigned char *buffer,
                                      std::size_t bufferSize,
                                      const std::chrono::steady_clock::time_point *timeout) override
    {
        std::size_t totalReadCount = 0;
        if(hasNextByte)
        {
            if(bufferSize == 0)
                return ReadBytesResult(0, false);
            *buffer++ = nextByte;
            bufferSize--;
            hasNextByte = false;
            totalReadCount++;
        }
        if(bufferSize > 0 && !hitEndOfFile)
        {
            auto readCount = zip_fread(zipFile, static_cast<void *>(buffer), bufferSize);
            if(readCount == 0)
            {
                hitEndOfFile = true;
                return ReadBytesResult(totalReadCount, hitEndOfFile);
            }
            if(readCount < 0)
            {
                std::cerr << "libzip error: zip_fread: " << zip_error_strerror(zip_get_error(zip))
                          << std::endl;
                abort();
            }
            totalReadCount += readCount;
        }
        auto readCount = zip_fread(zipFile, static_cast<void *>(&nextByte), 1);
        if(readCount < 0)
        {
            std::cerr << "libzip error: zip_fread: " << zip_error_strerror(zip_get_error(zip))
                      << std::endl;
            abort();
        }
        if(readCount == 0)
        {
            hitEndOfFile = true;
        }
        else
        {
            hasNextByte = true;
        }
        return ReadBytesResult(totalReadCount, hitEndOfFile);
    }
};

std::shared_ptr<io::InputStream> ResourceManager::readResource(const std::string &name)
{
    return std::make_shared<Implementation>(name);
}
}
}
}
