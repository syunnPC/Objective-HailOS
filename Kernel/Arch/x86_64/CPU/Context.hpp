#include "BaseType.hpp"

namespace Kernel::Arch::x86_64
{
    extern "C" void ArchSwitchContext(std::uint64_t* oldRSPOut, std::uint64_t newRSP) noexcept;
}