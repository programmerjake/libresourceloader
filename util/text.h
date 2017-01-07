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

#ifndef UTIL_TEXT_H_
#define UTIL_TEXT_H_

#include <type_traits>
#include <utility>
#include <limits>
#include <cstdint>
#include <string>
#include <ostream>
#include "constexpr_assert.h"

namespace programmerjake
{
namespace voxels
{
namespace util
{
namespace text
{
constexpr char32_t replacementCharacter = U'\uFFFD';

template <typename InputIterator, typename Sentinel>
typename std::char_traits<char32_t>::int_type decodeUTF8(
    InputIterator &iter,
    Sentinel sentinel,
    bool allowSurrogateCodePoints = true,
    bool allow2ByteNull = false,
    typename std::char_traits<char32_t>::int_type errorValue =
        replacementCharacter) noexcept(noexcept(++iter) && noexcept(static_cast<char>(*iter))
                                       && noexcept(iter == sentinel ? 0 : 0))
{
    if(iter == sentinel)
        return errorValue;
    auto byte0 = static_cast<std::uint8_t>(static_cast<char>(*iter));
    ++iter;
    if(byte0 < 0x80)
        return byte0;
    if(allow2ByteNull && byte0 == 0xC0)
    {
        if(iter == sentinel)
            return errorValue;
        auto byte1 = static_cast<std::uint8_t>(static_cast<char>(*iter));
        ++iter;
        if(byte1 != 0x80)
            return errorValue;
        return 0;
    }
    if(byte0 > 0xF4 || byte0 < 0xC2)
        return errorValue;
    if(iter == sentinel)
        return errorValue;
    auto byte1 = static_cast<std::uint8_t>(static_cast<char>(*iter));
    if(byte1 < 0x80 || byte1 >= 0xC0)
        return errorValue;
    if(byte0 < 0xE0)
    {
        ++iter;
        return (static_cast<std::uint_fast32_t>(byte0 & 0x1F) << 6) | (byte1 & 0x3F);
    }
    if(byte0 == 0xE0 && byte1 < 0xA0)
        return errorValue;
    if(byte0 == 0xF0 && byte1 < 0x90)
        return errorValue;
    if(byte0 == 0xF4 && byte1 >= 0x90)
        return errorValue;
    if(!allowSurrogateCodePoints && byte0 == 0xED && byte1 >= 0xA0)
        return errorValue;
    if(iter == sentinel)
        return errorValue;
    ++iter;
    auto byte2 = static_cast<std::uint8_t>(static_cast<char>(*iter));
    ++iter;
    if(byte2 < 0x80 || byte2 >= 0xC0)
        return errorValue;
    if(byte0 < 0xF0)
        return (static_cast<std::uint_fast32_t>(byte0 & 0xF) << 12)
               | (static_cast<std::uint_fast32_t>(byte1 & 0x3F) << 6) | (byte2 & 0x3F);
    if(iter == sentinel)
        return errorValue;
    auto byte3 = static_cast<std::uint8_t>(static_cast<char>(*iter));
    ++iter;
    if(byte3 < 0x80 || byte3 >= 0xC0)
        return errorValue;
    return (static_cast<std::uint_fast32_t>(byte0 & 0x7) << 18)
           | (static_cast<std::uint_fast32_t>(byte1 & 0x3F) << 12)
           | (static_cast<std::uint_fast32_t>(byte2 & 0x3F) << 6) | (byte3 & 0x3F);
}

template <typename T, std::size_t N>
struct EncodedCharacter final
{
    static constexpr std::size_t maxChars = N;
    typedef T CharType;
    static_assert(maxChars != 0, "");
    CharType chars[maxChars];
    std::size_t used;
    CharType &front()
    {
        return chars[0];
    }
    constexpr const CharType &front() const
    {
        return chars[0];
    }
    CharType &back()
    {
        return chars[0];
    }
    constexpr const CharType &back() const
    {
        return chars[0];
    }
    typedef const CharType *const_iterator;
    typedef CharType *iterator;
    constexpr const_iterator begin() const
    {
        return &chars[0];
    }
    constexpr const_iterator end() const
    {
        return begin() + used;
    }
    constexpr const_iterator cbegin() const
    {
        return &chars[0];
    }
    constexpr const_iterator cend() const
    {
        return begin() + used;
    }
    iterator begin()
    {
        return &chars[0];
    }
    iterator end()
    {
        return begin() + used;
    }
    constexpr std::size_t capacity() const
    {
        return maxChars;
    }
    constexpr std::size_t size() const
    {
        return used;
    }
    constexpr const CharType &operator[](std::size_t index) const
    {
        return (constexprAssert(index < used), chars[index]);
    }
    CharType &operator[](std::size_t index)
    {
        constexprAssert(index < used);
        return chars[index];
    }
    constexpr EncodedCharacter() : chars(), used(0)
    {
    }

private:
    static constexpr CharType implicitConversionHelper(CharType ch) noexcept
    {
        return ch;
    }

public:
    template <typename... Args>
    constexpr EncodedCharacter(Args &&... args)
        : chars{implicitConversionHelper(std::forward<Args>(args))...}, used(sizeof...(args))
    {
        static_assert(sizeof...(args) <= maxChars, "");
    }
    template <typename CharTraits, typename Allocator>
    operator std::basic_string<CharType, CharTraits, Allocator>() const
    {
        return std::basic_string<CharType, CharTraits, Allocator>(begin(), end());
    }
    template <typename CharTraits, typename Allocator>
    friend std::basic_string<CharType, CharTraits, Allocator> operator+(
        std::basic_string<CharType, CharTraits, Allocator> a, const EncodedCharacter &b)
    {
        a.append(b.begin(), b.end());
        return a;
    }
    template <typename CharTraits, typename Allocator>
    friend std::basic_string<CharType, CharTraits, Allocator> operator+(
        const EncodedCharacter &a, std::basic_string<CharType, CharTraits, Allocator> b)
    {
        b.insert(b.begin(), a.begin(), a.end());
        return b;
    }
    template <std::size_t N2>
    friend std::basic_string<CharType> operator+(const EncodedCharacter &a,
                                                 const EncodedCharacter<CharType, N2> &b)
    {
        std::basic_string<CharType> retval;
        retval.reserve(a.size() + b.size());
        retval.append(a.begin(), a.end());
        retval.append(b.begin(), b.end());
        return retval;
    }
    template <typename Traits>
    friend std::basic_ostream<CharType, Traits> &operator<<(
        std::basic_ostream<CharType, Traits> &os, const EncodedCharacter &a)
    {
        os << static_cast<std::basic_string<CharType, Traits>>(a);
        return os;
    }
};

EncodedCharacter<char, 4> encodeUTF8(char32_t ch, bool use2ByteNull = false) noexcept
{
    constexprAssert(ch < 0x10FFFFUL && ch >= 0);
    if(use2ByteNull && ch == 0)
        return EncodedCharacter<char, 4>(0xC0U, 0x80U);
    if(ch < 0x80)
        return EncodedCharacter<char, 4>(ch);
    if(ch < 0x800)
        return EncodedCharacter<char, 4>(0xC0 | (ch >> 6), 0x80 | (ch & 0x3F));
    if(ch < 0x10000UL)
        return EncodedCharacter<char, 4>(
            0xE0 | (ch >> 12), 0x80 | ((ch >> 6) & 0x3F), 0x80 | (ch & 0x3F));
    return EncodedCharacter<char, 4>(0xF0 | (ch >> 18),
                                     0x80 | ((ch >> 12) & 0x3F),
                                     0x80 | ((ch >> 6) & 0x3F),
                                     0x80 | (ch & 0x3F));
}

template <typename InputIterator, typename Sentinel>
typename std::char_traits<char32_t>::int_type decodeUTF16(
    InputIterator &iter,
    Sentinel sentinel,
    bool allowUnpairedSurrogateCodeUnits = true,
    typename std::char_traits<char32_t>::int_type errorValue =
        replacementCharacter) noexcept(noexcept(++iter) && noexcept(static_cast<char16_t>(*iter))
                                       && noexcept(iter == sentinel ? 0 : 0))
{
    if(iter == sentinel)
        return errorValue;
    auto unit0 = static_cast<std::uint16_t>(static_cast<char16_t>(*iter));
    ++iter;
    if(unit0 >= 0xD800U && unit0 < 0xDC00U)
    {
        if(iter == sentinel)
            return allowUnpairedSurrogateCodeUnits ? unit0 : errorValue;
        auto unit1 = static_cast<std::uint16_t>(static_cast<char16_t>(*iter));
        if(unit1 < 0xDC00U || unit1 >= 0xE000U)
            return allowUnpairedSurrogateCodeUnits ? unit0 : errorValue;
        ++iter;
        return 0x10000UL + ((unit0 & 0x3FF) << 10) + (unit1 & 0x3FF);
    }
    return unit0;
}

EncodedCharacter<char16_t, 2> encodeUTF16(char32_t ch) noexcept
{
    constexprAssert(ch < 0x10FFFFUL && ch >= 0);
    if(ch < 0x10000UL)
        return EncodedCharacter<char16_t, 2>(ch);
    return EncodedCharacter<char16_t, 2>(0xD800U | ((ch - 0x10000UL) >> 10),
                                         0xDC00U | ((ch - 0x10000UL) & 0x3FF));
}

template <typename InputIterator, typename Sentinel>
typename std::char_traits<char32_t>::int_type decodeUTF32(
    InputIterator &iter,
    Sentinel sentinel,
    bool allowSurrogateCodeUnits = true,
    typename std::char_traits<char32_t>::int_type errorValue =
        replacementCharacter) noexcept(noexcept(++iter) && noexcept(static_cast<char32_t>(*iter))
                                       && noexcept(iter == sentinel ? 0 : 0))
{
    if(iter == sentinel)
        return errorValue;
    auto retval = static_cast<std::uint32_t>(static_cast<char32_t>(*iter));
    ++iter;
    if(retval > 0x10FFFFUL)
        return errorValue;
    if(!allowSurrogateCodeUnits && retval >= 0xD800U && retval < 0xE000U)
        return errorValue;
    return retval;
}

EncodedCharacter<char32_t, 1> encodeUTF32(char32_t ch) noexcept
{
    return EncodedCharacter<char32_t, 1>(ch);
}

static_assert(std::numeric_limits<wchar_t>::radix == 2, "");
static_assert(std::numeric_limits<wchar_t>::digits
                      + static_cast<std::size_t>(std::is_signed<wchar_t>::value)
                  >= 16,
              "");

constexpr bool isWideCharacterUTF16 = std::numeric_limits<wchar_t>::digits <= 16;

EncodedCharacter<wchar_t, 2> encodeWide(char32_t ch) noexcept
{
    if(isWideCharacterUTF16)
    {
        auto result = encodeUTF16(ch);
        EncodedCharacter<wchar_t, 2> retval;
        retval.used = result.used;
        for(std::size_t i = 0; i < result.size(); i++)
        {
            retval[i] = static_cast<wchar_t>(result[i]);
        }
        return retval;
    }
    return EncodedCharacter<wchar_t, 2>(static_cast<wchar_t>(ch));
}

template <typename InputIterator, typename Sentinel>
typename std::char_traits<char32_t>::int_type decodeWide(
    InputIterator &iter,
    Sentinel sentinel,
    bool allowUnpairedSurrogateCodeUnits = true,
    typename std::char_traits<char32_t>::int_type errorValue =
        replacementCharacter) noexcept(noexcept(++iter) && noexcept(static_cast<wchar_t>(*iter))
                                       && noexcept(iter == sentinel ? 0 : 0))
{
    struct IteratorWrapper
    {
        InputIterator &iter;
        IteratorWrapper(InputIterator &iter) : iter(iter)
        {
        }
        void operator++()
        {
            ++iter;
        }
        wchar_t operator*()
        {
            return static_cast<wchar_t>(*iter);
        }
        bool operator==(Sentinel &sentinel)
        {
            return iter == sentinel;
        }
    };
    IteratorWrapper iteratorWrapper(iter);
    if(isWideCharacterUTF16)
        return decodeUTF16(
            iteratorWrapper, std::move(sentinel), allowUnpairedSurrogateCodeUnits, errorValue);
    return decodeUTF32(
        iteratorWrapper, std::move(sentinel), allowUnpairedSurrogateCodeUnits, errorValue);
}

struct ConvertOptions final
{
    typename std::char_traits<char32_t>::int_type errorValue = replacementCharacter;
    bool allowUnpairedSurrogateCodePoints = true;
    bool allow2ByteNull = false;
    bool use2ByteNull = false;
    constexpr ConvertOptions()
    {
    }
    constexpr ConvertOptions(typename std::char_traits<char32_t>::int_type errorValue,
                             bool allowUnpairedSurrogateCodePoints,
                             bool allow2ByteNull,
                             bool use2ByteNull)
        : errorValue(errorValue),
          allowUnpairedSurrogateCodePoints(allowUnpairedSurrogateCodePoints),
          allow2ByteNull(allow2ByteNull),
          use2ByteNull(use2ByteNull)
    {
    }
    static constexpr ConvertOptions strict(
        typename std::char_traits<char32_t>::int_type errorValue = replacementCharacter)
    {
        return ConvertOptions(errorValue, false, false, false);
    }
    static constexpr ConvertOptions java(
        typename std::char_traits<char32_t>::int_type errorValue = replacementCharacter)
    {
        return ConvertOptions(errorValue, true, true, true);
    }
};

template <typename CharType>
struct DecodeEncodeHelper
{
    template <typename InputIterator, typename Sentinel>
    typename std::char_traits<char32_t>::int_type decode(
        InputIterator &iter, Sentinel sentinel, const ConvertOptions &convertOptions) = delete;
    EncodedCharacter<CharType, 1> encode(char32_t ch,
                                         const ConvertOptions &convertOptions) noexcept = delete;
};

template <>
struct DecodeEncodeHelper<char>
{
    template <typename InputIterator, typename Sentinel>
    typename std::char_traits<char32_t>::int_type decode(
        InputIterator &iter,
        Sentinel sentinel,
        const ConvertOptions &
            convertOptions) noexcept(noexcept(decodeUTF8(std::declval<InputIterator &>(),
                                                         std::declval<Sentinel &&>())))
    {
        return decodeUTF8(iter,
                          std::move(sentinel),
                          convertOptions.allowUnpairedSurrogateCodePoints,
                          convertOptions.allow2ByteNull,
                          convertOptions.errorValue);
    }
    EncodedCharacter<char, 4> encode(char32_t ch, const ConvertOptions &convertOptions) noexcept
    {
        return encodeUTF8(ch, convertOptions.use2ByteNull);
    }
};

template <>
struct DecodeEncodeHelper<char16_t>
{
    template <typename InputIterator, typename Sentinel>
    typename std::char_traits<char32_t>::int_type decode(
        InputIterator &iter,
        Sentinel sentinel,
        const ConvertOptions &
            convertOptions) noexcept(noexcept(decodeUTF16(std::declval<InputIterator &>(),
                                                          std::declval<Sentinel &&>())))
    {
        return decodeUTF16(iter,
                           std::move(sentinel),
                           convertOptions.allowUnpairedSurrogateCodePoints,
                           convertOptions.errorValue);
    }
    EncodedCharacter<char16_t, 2> encode(char32_t ch, const ConvertOptions &convertOptions) noexcept
    {
        return encodeUTF16(ch);
    }
};

template <>
struct DecodeEncodeHelper<char32_t>
{
    template <typename InputIterator, typename Sentinel>
    typename std::char_traits<char32_t>::int_type decode(
        InputIterator &iter,
        Sentinel sentinel,
        const ConvertOptions &
            convertOptions) noexcept(noexcept(decodeUTF32(std::declval<InputIterator &>(),
                                                          std::declval<Sentinel &&>())))
    {
        return decodeUTF32(iter,
                           std::move(sentinel),
                           convertOptions.allowUnpairedSurrogateCodePoints,
                           convertOptions.errorValue);
    }
    EncodedCharacter<char32_t, 1> encode(char32_t ch, const ConvertOptions &convertOptions) noexcept
    {
        return encodeUTF32(ch);
    }
};

template <>
struct DecodeEncodeHelper<wchar_t>
{
    template <typename InputIterator, typename Sentinel>
    typename std::char_traits<char32_t>::int_type decode(
        InputIterator &iter,
        Sentinel sentinel,
        const ConvertOptions &
            convertOptions) noexcept(noexcept(decodeWide(std::declval<InputIterator &>(),
                                                         std::declval<Sentinel &&>())))
    {
        return decodeWide(iter,
                          std::move(sentinel),
                          convertOptions.allowUnpairedSurrogateCodePoints,
                          convertOptions.errorValue);
    }
    EncodedCharacter<wchar_t, 2> encode(char32_t ch, const ConvertOptions &convertOptions) noexcept
    {
        return encodeWide(ch);
    }
};

template <typename Target, typename Source>
struct StringCastHelper;

template <typename TargetCharType,
          typename TargetTraits,
          typename TargetAllocator,
          typename SourceCharType,
          typename SourceTraits,
          typename SourceAllocator>
struct StringCastHelper<std::basic_string<TargetCharType, TargetTraits, TargetAllocator>,
                        std::basic_string<SourceCharType, SourceTraits, SourceAllocator>>
{
    static std::basic_string<TargetCharType, TargetTraits, TargetAllocator> run(
        const std::basic_string<SourceCharType, SourceTraits, SourceAllocator> &source,
        const ConvertOptions &convertOptions)
    {
        std::basic_string<TargetCharType, TargetTraits, TargetAllocator> retval;
        for(auto iter = source.begin(); iter != source.end();)
        {
            retval = std::move(retval) + DecodeEncodeHelper<TargetCharType>::encode(
                                             DecodeEncodeHelper<SourceCharType>::decode(
                                                 iter, source.end(), convertOptions),
                                             convertOptions);
        }
        return retval;
    }
};

template <typename CharType,
          typename TargetTraits,
          typename TargetAllocator,
          typename SourceTraits,
          typename SourceAllocator>
struct StringCastHelper<std::basic_string<CharType, TargetTraits, TargetAllocator>,
                        std::basic_string<CharType, SourceTraits, SourceAllocator>>
{
    static std::basic_string<CharType, TargetTraits, TargetAllocator> run(
        const std::basic_string<CharType, SourceTraits, SourceAllocator> &source,
        const ConvertOptions &)
    {
        return std::basic_string<CharType, TargetTraits, TargetAllocator>(source.begin(),
                                                                          source.end());
    }
};

template <typename CharType, typename Traits, typename Allocator>
struct StringCastHelper<std::basic_string<CharType, Traits, Allocator>,
                        std::basic_string<CharType, Traits, Allocator>>
{
    static std::basic_string<CharType, Traits, Allocator> run(
        const std::basic_string<CharType, Traits, Allocator> &source, const ConvertOptions &)
    {
        return source;
    }
    static std::basic_string<CharType, Traits, Allocator> run(
        std::basic_string<CharType, Traits, Allocator> &&source, const ConvertOptions &)
    {
        return std::move(source);
    }
};

template <typename Target, typename Source>
Target stringCast(Source &&source, const ConvertOptions &convertOptions)
{
    return StringCastHelper<Target, typename std::decay<Source>::type>::run(
        std::forward<Source>(source), convertOptions);
}

template <typename Target, typename Source>
Target stringCast(Source &&source)
{
    return StringCastHelper<Target, typename std::decay<Source>::type>::run(
        std::forward<Source>(source), ConvertOptions());
}
}
}
}
}

#endif /* UTIL_TEXT_H_ */
