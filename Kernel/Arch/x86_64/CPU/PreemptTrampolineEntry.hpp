#pragma once

#include "BaseType.hpp"

extern "C" void PreemptTrampolineEntry() noexcept;
extern "C" void PreemptTrampolineBegin() noexcept;
extern "C" void PreemptTrampolineEnd()   noexcept;
extern "C" void SchedYieldTrampoline() noexcept;
extern "C" std::uint64_t GetPreemptResumeRIP() noexcept;
extern "C" void PreemptDisableForTrampoline() noexcept;
extern "C" void PreemptEnableForTrampoline() noexcept;

extern "C" void PreemptSetSavedRSP(std::uint64_t v) noexcept;
extern "C" std::uint64_t PreemptGetSavedRSP() noexcept;
