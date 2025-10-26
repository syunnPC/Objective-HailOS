#pragma once

#include <cstddef>
#include <cstdint>
#include <climits>

constexpr uint32_t STATUS_ERROR_BASE = 0x8000'0000;

#define ERROR(status) (static_cast<std::uint32_t>(status) >= STATUS_ERROR_BASE)
#define SUCCESS(status) (static_cast<std::uint32_t>(status) < STATUS_ERROR_BASE)

enum class Status : uint32_t
{
    //TODO:必要に応じて追加、サブシステムタイプの分類も考える
    STATUS_SUCCESS = 0x0,
    STATUS_ERROR = 0x8000'0000,
};