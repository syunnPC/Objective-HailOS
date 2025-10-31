#include "UUID.hpp"
#include "MemoryUtility.hpp"

namespace Kernel
{
    UUID::UUID(IRandomDevice& rd) noexcept
    {
        std::uint64_t* p = reinterpret_cast<std::uint64_t*>(m_Bytes);
        p[0] = rd.Generate64();
        p[1] = rd.Generate64();
    }

    UUID::UUID(const UUID& v) noexcept
    {
        Library::Memory::CopyMemory(v.m_Bytes, m_Bytes, sizeof(m_Bytes));
    }

    UUID::UUID(UUID&& v) noexcept
    {
        Library::Memory::CopyMemory(v.m_Bytes, m_Bytes, sizeof(m_Bytes));
    }

    UUID::UUID(const std::uint8_t* bytes) noexcept
    {
        Library::Memory::CopyMemory(bytes, m_Bytes, sizeof(m_Bytes));
    }

    const std::uint8_t* UUID::Bytes() const noexcept
    {
        return m_Bytes;
    }

    bool UUID::operator==(const IUUID& rhs) const noexcept
    {
        return Library::Memory::CompareMemory(rhs.Bytes(), m_Bytes, sizeof(m_Bytes));
    }

    bool UUID::operator!=(const IUUID& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    std::size_t UUID::ToString(char* buf, std::size_t size) noexcept
    {
        //TODO:実装
        return 0;
    }
}