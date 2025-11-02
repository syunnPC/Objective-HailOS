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
        char Magic[4];
        std::uint32_t AllocatedPages;
        void* Base;
    };
}

namespace
{
    static inline std::uintptr_t AlignUp(std::uintptr_t p, std::size_t a) noexcept
    {
        const std::uintptr_t m = static_cast<std::uintptr_t>(a - 1);
        return (p + m) & ~m;
    }

    static inline bool IsPow2(std::size_t x) noexcept
    {
        return x && ((x & (x - 1)) == 0);
    }

    static inline std::size_t ToPages(std::size_t bytes) noexcept
    {
        const auto ps = Kernel::PAGE_SIZE;
        return (bytes + ps - 1) / ps;
    }

    static inline bool IsKMEMHeader(const Kernel::Early::AllocatedMemoryInfo* hdr) noexcept
    {
        return hdr && hdr->Magic[0] == 'K' && hdr->Magic[1] == 'M' && hdr->Magic[2] == 'E' && hdr->Magic[3] == 'M';
    }

    [[noreturn]] static inline void PanicNoMem()
    {
        PANIC(Status::STATUS_ERROR, 0);
    }

    static inline void* AllocImpl(std::size_t size, std::size_t align, bool nothrow) noexcept
    {
        using Kernel::Early::AllocatedMemoryInfo;

        if (!Kernel::Early::gEarlyAlloc)
        {
            if (nothrow) return nullptr;
            PanicNoMem();
        }

        if (align < alignof(std::max_align_t))
        {
            align = alignof(std::max_align_t);
        }

        if (!IsPow2(align))
        {
            std::size_t a = alignof(std::max_align_t);
            while (a < align) a <<= 1;
            align = a;
        }

        const std::size_t total = sizeof(AllocatedMemoryInfo) + (align - 1) + size;
        void* const base = Kernel::Early::gEarlyAlloc->AllocatePage(total);
        if (!base)
        {
            if (nothrow) return nullptr;
            PanicNoMem();
        }

        const auto base_u = reinterpret_cast<std::uintptr_t>(base);
        const auto aligned_payload_u = AlignUp(base_u + sizeof(AllocatedMemoryInfo), align);

        auto* const hdr = reinterpret_cast<AllocatedMemoryInfo*>(aligned_payload_u - sizeof(AllocatedMemoryInfo));
        hdr->Magic[0] = 'K'; hdr->Magic[1] = 'M'; hdr->Magic[2] = 'E'; hdr->Magic[3] = 'M';
        hdr->AllocatedPages = static_cast<std::uint32_t>(ToPages(total));
        hdr->Base = base;

        return reinterpret_cast<void*>(aligned_payload_u);
    }

    static inline void KFreeImpl(void* p) noexcept
    {
        using Kernel::Early::AllocatedMemoryInfo;

        if (!p)
        {
            return;
        }

        auto* const hdr = reinterpret_cast<AllocatedMemoryInfo*>(reinterpret_cast<std::uintptr_t>(p) - sizeof(AllocatedMemoryInfo));

        if (!IsKMEMHeader(hdr))
        {
            return;
        }

        if (!hdr->Base || hdr->AllocatedPages == 0)
        {
            return;
        }

        Kernel::Early::gEarlyAlloc->FreePage(hdr->Base, hdr->AllocatedPages);
    }
}

void* operator new(std::size_t size)
{
    return AllocImpl(size, alignof(std::max_align_t), false);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    return AllocImpl(size, alignof(std::max_align_t), true);
}

void operator delete(void* p) noexcept
{
    KFreeImpl(p);
}

void operator delete(void* p, const std::nothrow_t&) noexcept
{
    KFreeImpl(p);
}

void operator delete(void* p, std::size_t) noexcept
{
    KFreeImpl(p);
}

void* operator new(std::size_t size, std::align_val_t al)
{
    return AllocImpl(size, static_cast<std::size_t>(al), false);
}

void* operator new(std::size_t size, std::align_val_t al, const std::nothrow_t&) noexcept
{
    return AllocImpl(size, static_cast<std::size_t>(al), true);
}

void operator delete(void* p, std::align_val_t) noexcept
{
    KFreeImpl(p);
}

void operator delete(void* p, std::size_t, std::align_val_t) noexcept
{
    KFreeImpl(p);
}

void* operator new[](std::size_t size)
{
    return ::operator new(size);
}

void* operator new[](std::size_t size, const std::nothrow_t& nt) noexcept
{
    return ::operator new(size, nt);
}

void* operator new[](std::size_t size, std::align_val_t al)
{
    return ::operator new(size, al);
}

void* operator new[](std::size_t size, std::align_val_t al, const std::nothrow_t& nt) noexcept
{
    return ::operator new(size, al, nt);
}

void operator delete[](void* p) noexcept
{
    ::operator delete(p);
}

void operator delete[](void* p, const std::nothrow_t&) noexcept
{
    ::operator delete(p);
}

void operator delete[](void* p, std::size_t) noexcept
{
    ::operator delete(p);
}

void operator delete[](void* p, std::align_val_t) noexcept
{
    ::operator delete(p);
}

void operator delete[](void* p, std::size_t, std::align_val_t) noexcept
{
    ::operator delete(p);
}