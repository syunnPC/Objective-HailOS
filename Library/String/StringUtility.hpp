#pragma once

#include "BaseType.hpp"

namespace Library::String
{
    inline std::size_t StringLength(const char* str)
    {
        std::size_t i = 0;
        while (true)
        {
            if (str[i] == '\0')
            {
                return i;
            }
            ++i;
        }
    }

    // スレッドセーフとかじゃないので注意
    inline const char* ULongToHexString(std::uint64_t n)
    {
        static char buf[19];
        static const char* hex = "0123456789ABCDEF";

        buf[0] = '0';
        buf[1] = 'x';

        for (int i = 0; i < 16; i++)
        {
            int shift = (15 - i) * 4;
            std::uint8_t nib = (n >> shift) & 0xF;
            buf[2 + i] = hex[nib];
        }

        buf[18] = '\0';
        return buf;
    }
}