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
#include "stream_base.h"
#include <unordered_map>
#include <atomic>

namespace programmerjake
{
namespace voxels
{
namespace io
{
struct StreamBase::KeyValueMapImplementation final
{
    std::unordered_map<std::size_t, std::shared_ptr<void>> map;
};

void StreamBase::freeKeyValueMap(KeyValueMapImplementation *keyValueMap) noexcept
{
    delete keyValueMap;
}

std::shared_ptr<void> StreamBase::getKeyedValue(std::size_t id) const noexcept
{
    if(!keyValueMap)
        return nullptr;
    auto iter = keyValueMap->map.find(id);
    if(iter == keyValueMap->map.end())
        return nullptr;
    return std::get<1>(*iter);
}

void StreamBase::setKeyedValue(std::size_t id, std::shared_ptr<void> newValue)
{
    if(!keyValueMap)
        keyValueMap = new KeyValueMapImplementation;
    keyValueMap->map[id] = newValue;
}

std::size_t StreamBase::allocateKeyedValueId() noexcept
{
    static std::atomic_size_t nextId(0);
    return nextId.fetch_add(1, std::memory_order_relaxed);
}
}
}
}
