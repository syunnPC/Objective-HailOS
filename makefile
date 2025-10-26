#もしこのmakefileがちゃんと動かない（cstdlibなどの探索でエラーが出る）ときは
#以下のコマンドラインを試す（gcc/g++のインストールが必要）
# make clean && make V=1   STDCPP_INC=/home/linuxbrew/.linuxbrew/Cellar/gcc/15.2.0/include/c++/15   STDCPP_INC_TRIPLE=/home/linuxbrew/.linuxbrew/Cellar/gcc/15.2.0/include/c++/15/x86_64-pc-linux-gnu
#x86_64-elf-gccにC++標準ライブラリがないのが原因だと思う

TARGET_ARCH = x86_64
TARGET_TRIPLET = $(TARGET_ARCH)-elf
CXX = $(TARGET_TRIPLET)-g++
CC  = $(TARGET_TRIPLET)-gcc
AS  = $(TARGET_TRIPLET)-as
LD  = $(TARGET_TRIPLET)-ld
OBJCOPY = $(TARGET_TRIPLET)-objcopy

BUILD_DIR = Build
OBJ_DIR   = $(BUILD_DIR)/obj
KERNEL_ELF = $(BUILD_DIR)/kernel.elf

KERNEL_DIR  = Kernel
LIBRARY_DIR = Library

KERNEL_ARCH_DIR      = $(KERNEL_DIR)/Arch/x86_64
KERNEL_BOOT_DIR      = $(KERNEL_ARCH_DIR)/Boot
KERNEL_APIC_DIR      = $(KERNEL_ARCH_DIR)/APIC
KERNEL_CPU_DIR       = $(KERNEL_ARCH_DIR)/CPU
KERNEL_CPU_INT_DIR   = $(KERNEL_CPU_DIR)/Interrupt
KERNEL_IO_DIR        = $(KERNEL_ARCH_DIR)/IO
KERNEL_PAGING_DIR    = $(KERNEL_ARCH_DIR)/Paging
KERNEL_UTIL_DIR      = $(KERNEL_ARCH_DIR)/Utility

CORE_DIR         = $(KERNEL_DIR)/Core
CORE_ACPI_DIR    = $(CORE_DIR)/ACPI
CORE_ERR_DIR     = $(CORE_DIR)/ErrorHandling
CORE_CONSOLE_DIR = $(CORE_DIR)/KernelConsole
CORE_MEM_DIR     = $(CORE_DIR)/Memory
CORE_TIMER_DIR   = $(CORE_DIR)/Timer

INCLUDE_DIR             = $(KERNEL_DIR)/Include
INCLUDE_CORE_DIR        = $(INCLUDE_DIR)/Core
INCLUDE_CORE_RAND_DIR   = $(INCLUDE_CORE_DIR)/RandomDevice
INCLUDE_CORE_UUID_DIR   = $(INCLUDE_CORE_DIR)/UUID
INCLUDE_CORE_MEMORY_DIR = $(INCLUDE_CORE_DIR)/Memory
INCLUDE_OBJECT_DIR      = $(INCLUDE_DIR)/Object
INCLUDE_OBJECT_IF_DIR   = $(INCLUDE_OBJECT_DIR)/Interface
INCLUDE_UTIL_DIR        = $(INCLUDE_DIR)/Utility

LIB_MEMORY_DIR = $(LIBRARY_DIR)/Memory
LIB_STRING_DIR = $(LIBRARY_DIR)/String

CXXFLAGS_COMMON = -g -ffreestanding -Wall -Wextra -mno-red-zone -mcmodel=kernel -fno-pie -fno-stack-protector -O0 -std=c++23 -fno-exceptions -fno-rtti
CXXFLAGS = $(CXXFLAGS_COMMON)
CFLAGS   = -g -ffreestanding -Wall -Wextra -mno-red-zone -mcmodel=kernel -fno-pie -fno-stack-protector -O0 -std=c11
ASFLAGS  = -g

CXXFLAGS_INTERRUPT = $(CXXFLAGS_COMMON) -mgeneral-regs-only

INCLUDES = \
  -I$(KERNEL_DIR) \
  -I$(KERNEL_ARCH_DIR) \
  -I$(KERNEL_APIC_DIR) \
  -I$(KERNEL_BOOT_DIR) \
  -I$(KERNEL_CPU_DIR) \
  -I$(KERNEL_CPU_INT_DIR) \
  -I$(KERNEL_IO_DIR) \
  -I$(KERNEL_PAGING_DIR) \
  -I$(KERNEL_UTIL_DIR) \
  -I$(CORE_DIR) \
  -I$(CORE_ACPI_DIR) \
  -I$(CORE_ERR_DIR) \
  -I$(CORE_CONSOLE_DIR) \
  -I$(CORE_MEM_DIR) \
  -I$(CORE_TIMER_DIR) \
  -I$(INCLUDE_DIR) \
  -I$(INCLUDE_CORE_DIR) \
  -I$(INCLUDE_CORE_RAND_DIR) \
  -I$(INCLUDE_CORE_UUID_DIR) \
  -I$(INCLUDE_CORE_MEMORY_DIR) \
  -I$(INCLUDE_OBJECT_DIR) \
  -I$(INCLUDE_OBJECT_IF_DIR) \
  -I$(INCLUDE_UTIL_DIR) \
  -I$(LIBRARY_DIR) \
  -I$(LIB_MEMORY_DIR) \
  -I$(LIB_STRING_DIR)

LDSCRIPT = linker.ld
LDFLAGS = -T $(LDSCRIPT) -nostdlib -n

ASM_SOURCES = \
  $(KERNEL_BOOT_DIR)/Boot.S

CPP_SOURCE_INTERRUPT = $(KERNEL_CPU_INT_DIR)/InterruptDispatch.cpp

CPP_SOURCES = \
  $(KERNEL_APIC_DIR)/APICController.cpp \
  $(KERNEL_APIC_DIR)/LAPIC.cpp \
  $(KERNEL_CPU_DIR)/CPU_Init.cpp \
  $(KERNEL_PAGING_DIR)/Paging.cpp \
  $(KERNEL_UTIL_DIR)/CriticalSection.cpp \
  $(CORE_ACPI_DIR)/ACPIManager.cpp \
  $(CORE_ERR_DIR)/Panic.cpp \
  $(CORE_CONSOLE_DIR)/KernelConsole.cpp \
  $(CORE_DIR)/KernelMain.cpp \
  $(CORE_DIR)/CppRuntime.cpp \
  $(CORE_MEM_DIR)/EarlyPageAllocator.cpp \
  $(CORE_MEM_DIR)/NewDelete.cpp \
  $(CORE_MEM_DIR)/PhysicalMemoryBitmap.cpp \
  $(CORE_TIMER_DIR)/Timer.cpp \
  $(INCLUDE_CORE_RAND_DIR)/HardwareRandomDevice.cpp \
  $(INCLUDE_CORE_UUID_DIR)/UUID.cpp \
  $(LIB_MEMORY_DIR)/MemoryUtility.cpp

C_SOURCES = \
  $(CORE_CONSOLE_DIR)/ConsoleFont.c

OBJ_FILES_ASM = $(patsubst %.S,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
OBJ_FILES_CPP = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(filter-out $(CPP_SOURCE_INTERRUPT),$(CPP_SOURCES)))
OBJ_FILE_INTERRUPT = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCE_INTERRUPT))
OBJ_FILES_C = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))

ALL_OBJS = $(OBJ_FILES_ASM) $(OBJ_FILES_CPP) $(OBJ_FILE_INTERRUPT) $(OBJ_FILES_C)

HOSTCXX        ?= g++
HOST_TRIPLE    := $(shell $(HOSTCXX) -dumpmachine)
HOST_GCC_FULL  := $(shell $(HOSTCXX) -dumpfullversion 2>/dev/null || $(HOSTCXX) -dumpversion)
HOST_GCC_MAJOR := $(shell echo $(HOST_GCC_FULL) | cut -d. -f1)
HOST_CXXINC_RT := $(shell $(HOSTCXX) -print-file-name=include/c++)

HOST_INC_VER_DIR := $(firstword \
  $(wildcard $(HOST_CXXINC_RT)/$(HOST_GCC_FULL)) \
  $(wildcard $(HOST_CXXINC_RT)/$(HOST_GCC_MAJOR)) \
)

CXXCFG_BITS_CAND := $(wildcard $(HOST_INC_VER_DIR)/*/bits/c++config.h)
STDCXX_TRIPLE_DIR := $(patsubst %/bits/c++config.h,%,$(firstword $(CXXCFG_BITS_CAND)))

STDCXX_SYSINC :=
ifneq ($(strip $(HOST_INC_VER_DIR)),)
  STDCXX_SYSINC += -isystem $(HOST_INC_VER_DIR)
endif
ifneq ($(strip $(STDCXX_TRIPLE_DIR)),)
  STDCXX_SYSINC += -isystem $(STDCXX_TRIPLE_DIR)
endif

ifneq ($(strip $(STDCPP_INC)),)
  STDCXX_SYSINC += -isystem $(STDCPP_INC)
endif
ifneq ($(strip $(STDCPP_INC_TRIPLE)),)
  STDCXX_SYSINC += -isystem $(STDCPP_INC_TRIPLE)
endif

ifneq ("$(wildcard /usr/include/features.h)","")
  STDCXX_SYSINC += -isystem /usr/include
else ifneq ("$(wildcard /usr/include/x86_64-linux-gnu/features.h)","")
  STDCXX_SYSINC += -isystem /usr/include/x86_64-linux-gnu
else ifneq ("$(wildcard /home/linuxbrew/.linuxbrew/include/features.h)","")
  STDCXX_SYSINC += -isystem /home/linuxbrew/.linuxbrew/include
endif

ifneq ("$(wildcard /usr/include/x86_64-linux-gnu/sys/cdefs.h)","")
  STDCXX_SYSINC += -isystem /usr/include/x86_64-linux-gnu
else ifneq ("$(wildcard /usr/include/sys/cdefs.h)","")
  STDCXX_SYSINC += -isystem /usr/include
else ifneq ("$(wildcard /home/linuxbrew/.linuxbrew/include/sys/cdefs.h)","")
  STDCXX_SYSINC += -isystem /home/linuxbrew/.linuxbrew/include
endif

export CPLUS_INCLUDE_PATH := $(patsubst -isystem %,%, $(STDCXX_SYSINC)):$(CPLUS_INCLUDE_PATH)

.PHONY: all clean
all: $(KERNEL_ELF)

$(KERNEL_ELF): $(ALL_OBJS) $(LDSCRIPT)
	@echo "LD   $@"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(ALL_OBJS) -lgcc

$(OBJ_DIR)/%.o: %.cpp
	@echo "CXX  $<"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(STDCXX_SYSINC) $(INCLUDES) -c $< -o $@

$(OBJ_FILE_INTERRUPT): $(CPP_SOURCE_INTERRUPT)
	@echo "CXX-INT $<"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_INTERRUPT) $(STDCXX_SYSINC) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@echo "CC   $<"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: %.S
	@echo "AS   $<"
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)