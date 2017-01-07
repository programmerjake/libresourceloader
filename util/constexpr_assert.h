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

#ifndef UTIL_CONSTEXPR_ASSERT_H_
#define UTIL_CONSTEXPR_ASSERT_H_

#include <cassert>

namespace programmerjake
{
namespace voxels
{
namespace util
{
struct ConstexprAssertFailed final
{
    template <typename Fn>
    ConstexprAssertFailed(Fn &&fn)
    {
        fn();
    }
};

#ifdef NDEBUG
#define constexprAssert(v) (static_cast<void>(decltype((v) ? 0 : 0)()))
#else
#define constexprAssert(v)                                                            \
    ((v) ? static_cast<void>(0) :                                                     \
           static_cast<void>(::programmerjake::voxels::util::ConstexprAssertFailed([] \
                                                                                   {  \
        assert(!#v);                                                                  \
                                                                                   })))
#endif
}
}
}

#endif /* UTIL_CONSTEXPR_ASSERT_H_ */
