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

#ifndef IO_STREAM_BASE_H_
#define IO_STREAM_BASE_H_

#include <memory>
#include <type_traits>
#include <system_error>
#include <string>

namespace programmerjake
{
namespace voxels
{
namespace io
{
struct IOError : public std::system_error
{
    IOError(std::error_code errorCode) : std::system_error(errorCode)
    {
    }
    IOError(std::error_code errorCode, const std::string &errorMessage)
        : std::system_error(errorCode, errorMessage)
    {
    }
    IOError(std::error_code errorCode, const char *errorMessage)
        : std::system_error(errorCode, errorMessage)
    {
    }
    IOError(int errorValue, const std::error_category &errorCategory)
        : std::system_error(errorValue, errorCategory)
    {
    }
    IOError(int errorValue,
            const std::error_category &errorCategory,
            const std::string &errorMessage)
        : std::system_error(errorValue, errorCategory, errorMessage)
    {
    }
    IOError(int errorValue, const std::error_category &errorCategory, const char *errorMessage)
        : std::system_error(errorValue, errorCategory, errorMessage)
    {
    }
};

struct EOFError : public IOError
{
    EOFError() : EOFError(std::make_error_code(std::errc::io_error))
    {
    }
    EOFError(std::error_code errorCode) : IOError(errorCode, "unexpected end-of-file")
    {
    }
    EOFError(int errorValue, const std::error_category &errorCategory)
        : EOFError(std::error_code(errorValue, errorCategory))
    {
    }
};

class StreamBase : public std::enable_shared_from_this<StreamBase>
{
private:
    struct KeyValueMapImplementation;
    template <typename T, typename Tag>
    static std::size_t getKeyedValueId() noexcept
    {
        static std::size_t retval = allocateKeyedValueId();
        return retval;
    }

private:
    static void freeKeyValueMap(KeyValueMapImplementation *keyValueMap) noexcept;
    std::shared_ptr<void> getKeyedValue(std::size_t id) const noexcept;
    void setKeyedValue(std::size_t id, std::shared_ptr<void> newValue);
    static std::size_t allocateKeyedValueId() noexcept;

private:
    KeyValueMapImplementation *keyValueMap = nullptr;

public:
    virtual ~StreamBase()
    {
        if(keyValueMap)
            freeKeyValueMap(keyValueMap);
    }
    constexpr StreamBase() = default;
    StreamBase(StreamBase &&rt) noexcept : keyValueMap(rt.keyValueMap)
    {
        rt.keyValueMap = nullptr;
    }
    StreamBase &operator=(StreamBase rt) noexcept
    {
        if(keyValueMap)
            freeKeyValueMap(keyValueMap);
        keyValueMap = rt.keyValueMap;
        rt.keyValueMap = nullptr;
        return *this;
    }
    template <typename T, typename Tag>
    std::shared_ptr<T> getKeyedValue() const noexcept
    {
        return std::static_pointer_cast<T>(getKeyedValue(getKeyedValueId<T, Tag>()));
    }
    template <typename T, typename Tag>
    void setKeyedValue(const typename std::enable_if<true, std::shared_ptr<T>>::type &newValue)
    {
        setKeyedValue(
            getKeyedValueId<T, Tag>(),
            std::const_pointer_cast<void>(std::static_pointer_cast<const void>(newValue)));
    }
};
}
}
}

#endif /* IO_STREAM_BASE_H_ */
