#include "BaseType.hpp"

namespace Library::Memory
{
    void FillMemory(void* ptr, std::size_t size, std::uint8_t value)
    {
        std::uint8_t* p = reinterpret_cast<std::uint8_t*>(ptr);
        for (std::size_t i = 0; i < size; ++i)
        {
            p[i] = value;
        }
    }

    void CopyMemory(const void* src, void* dest, std::size_t size)
    {
        std::uint8_t* pd = reinterpret_cast<std::uint8_t*>(dest);
        const std::uint8_t* ps = reinterpret_cast<const std::uint8_t*>(src);
        for (std::size_t i = 0; i < size; ++i)
        {
            pd[i] = ps[i];
        }
    }

    bool CompareMemory(const void* ptr1, const void* ptr2, std::size_t size)
    {
        const std::uint8_t* p1 = reinterpret_cast<const std::uint8_t*>(ptr1);
        const std::uint8_t* p2 = reinterpret_cast<const std::uint8_t*>(ptr2);
        for (std::size_t i = 0; i < size; ++i)
        {
            if (p1[i] != p2[i])
            {
                return false;
            }
        }

        return true;
    }
}