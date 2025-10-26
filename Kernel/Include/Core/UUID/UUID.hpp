#pragma once

#include "RefCountedBase.hpp"
#include "IUUID.hpp"
#include "IRandomDevice.hpp"

namespace Kernel
{
    class UUID final : public RefCountedBase, public virtual IUUID
    {
    private:
        std::uint8_t m_Bytes[16];
    public:
        UUID() = delete;
        UUID(IRandomDevice& rd) noexcept;
        UUID(const UUID& v) noexcept;
        UUID(UUID&& v) noexcept;
        UUID(const std::uint8_t* bytes) noexcept;
        virtual const std::uint8_t* Bytes() const noexcept override;
        virtual bool operator==(const IUUID& rhs) const noexcept override;
        virtual bool operator!=(const IUUID& rhs) const noexcept override;
        virtual std::size_t ToString(char* buf, std::size_t size) noexcept override;
    protected:
        virtual ~UUID() override = default;
    };
}