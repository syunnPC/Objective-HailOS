#include <new>

#include "MemoryInfo.hpp"
#include "EarlyPageAllocator.hpp"
#include "Panic.hpp"
#include "MemoryUtility.hpp"

namespace Kernel::Early
{
    static IPageAllocator* gEarlyAlloc = nullptr;

    void InitOperatorNew(IPageAllocator& alloc) noexcept
    {
        gEarlyAlloc = &alloc;
    }

    struct AllocatedMemoryInfo
    {
        char Magic[4]; //"KMEM"
        std::uint32_t AllocatedPages;
    } __attribute__((packed));
}

void* operator new(std::size_t size)
{
    std::size_t sizeAlloc = size + sizeof(Kernel::Early::AllocatedMemoryInfo);

    auto p = Kernel::Early::gEarlyAlloc->AllocatePage(sizeAlloc);
    if (p == nullptr) [[unlikely]]
    {
        PANIC(Status::STATUS_ERROR, 0);
    }

    *(reinterpret_cast<Kernel::Early::AllocatedMemoryInfo*>(p) - 1) = { {'K', 'M', 'E', 'M'}, static_cast<std::uint32_t>((sizeAlloc % Kernel::PAGE_SIZE != 0) ? sizeAlloc / Kernel::PAGE_SIZE + 1 : sizeAlloc / Kernel::PAGE_SIZE) };
    return p;
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    std::size_t sizeAlloc = size + sizeof(Kernel::Early::AllocatedMemoryInfo);

    auto p = Kernel::Early::gEarlyAlloc->AllocatePage(sizeAlloc);
    if (p == nullptr) [[unlikely]]
    {
        return nullptr;
    }

    *(reinterpret_cast<Kernel::Early::AllocatedMemoryInfo*>(p) - 1) = { {'K', 'M', 'E', 'M'}, static_cast<std::uint32_t>((sizeAlloc % Kernel::PAGE_SIZE != 0) ? sizeAlloc / Kernel::PAGE_SIZE + 1 : sizeAlloc / Kernel::PAGE_SIZE) };
    return p;
}

void operator delete(void* p) noexcept
{
    auto allocatedAddr = reinterpret_cast<Kernel::Early::AllocatedMemoryInfo*>(p) - 1;
    auto page = allocatedAddr->AllocatedPages;
    Kernel::Early::gEarlyAlloc->FreePage(allocatedAddr, page);
}

void operator delete(void* p, const std::nothrow_t&) noexcept
{
    auto allocatedAddr = reinterpret_cast<Kernel::Early::AllocatedMemoryInfo*>(p) - 1;
    if (!Library::Memory::CompareMemory(allocatedAddr->Magic, "KMEM", sizeof(allocatedAddr->Magic)))
    {
        return;
    }

    auto page = allocatedAddr->AllocatedPages;
    Kernel::Early::gEarlyAlloc->FreePage(allocatedAddr, page);
}

void* operator new[](std::size_t size)
{
    return ::operator new(size);
}

void* operator new[](std::size_t size, const std::nothrow_t& n) noexcept
{
    return ::operator new(size, n);
}

void operator delete[](void* p) noexcept
{
    ::operator delete(p);
}

void operator delete[](void* p, const std::nothrow_t&) noexcept
{
    return ::operator delete(p);
}

void operator delete(void* p, std::size_t) noexcept
{
    ::operator delete(p);
}

void operator delete[](void* p, std::size_t) noexcept
{
    ::operator delete[](p);
}