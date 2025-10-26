#pragma once

#include "BaseType.hpp"

namespace Library::Memory
{
    void FillMemory(void* ptr, std::size_t size, std::uint8_t value);
    void CopyMemory(const void* src, void* dest, std::size_t size);
    bool CompareMemory(const void* ptr1, const void* ptr2, std::size_t size);
}