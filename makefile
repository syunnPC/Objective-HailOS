TARGET_ARCH   = x86_64
TARGET_TRIPLET= $(TARGET_ARCH)-elf
CXX = $(TARGET_TRIPLET)-g++
CC  = $(TARGET_TRIPLET)-gcc
LD  = $(TARGET_TRIPLET)-ld

BUILD_DIR = Build
OBJ_DIR   = $(BUILD_DIR)/obj
KERNEL_ELF= $(BUILD_DIR)/kernel.elf

CXXFLAGS_COMMON = -g -ffreestanding -Wall -Wextra -mno-red-zone -mcmodel=kernel -fno-pie -fno-stack-protector -O0 -std=c++23 -fno-exceptions -fno-rtti
CXXFLAGS          = $(CXXFLAGS_COMMON)
CXXFLAGS_INTERRUPT= $(CXXFLAGS_COMMON) -mgeneral-regs-only
CFLAGS            = -g -ffreestanding -Wall -Wextra -mno-red-zone -mcmodel=kernel -fno-pie -fno-stack-protector -O0 -std=c11
ASFLAGS           = -g
LDSCRIPT          = linker.ld
LDFLAGS           = -T $(LDSCRIPT) -nostdlib -n

TP_STDXX_ROOT_A     := ThirdParty/stdcxx
TP_CXX_INC_A        := $(TP_STDXX_ROOT_A)/cxx
TP_CXX_TRIPLE_INC_A := $(TP_STDXX_ROOT_A)/cxx_triple
TP_GLIBC_INC_A      := $(TP_STDXX_ROOT_A)/glibc

TP_STDXX_ROOT_B     := ThirdParty
TP_CXX_INC_B        := $(TP_STDXX_ROOT_B)/cxx
TP_CXX_TRIPLE_INC_B := $(TP_STDXX_ROOT_B)/cxx_triple
TP_GLIBC_INC_B      := $(TP_STDXX_ROOT_B)/glibc

STDCXX_SYSINC :=

ifneq ("$(wildcard $(TP_CXX_INC_A))","")
  STDCXX_SYSINC += -isystem $(TP_CXX_INC_A)
endif
ifneq ("$(wildcard $(TP_CXX_TRIPLE_INC_A))","")
  STDCXX_SYSINC += -isystem $(TP_CXX_TRIPLE_INC_A)
endif
ifneq ("$(wildcard $(TP_GLIBC_INC_A))","")
  STDCXX_SYSINC += -isystem $(TP_GLIBC_INC_A)
endif

ifneq ("$(wildcard $(TP_CXX_INC_B))","")
  STDCXX_SYSINC += -isystem $(TP_CXX_INC_B)
endif
ifneq ("$(wildcard $(TP_CXX_TRIPLE_INC_B))","")
  STDCXX_SYSINC += -isystem $(TP_CXX_TRIPLE_INC_B)
endif
ifneq ("$(wildcard $(TP_GLIBC_INC_B))","")
  STDCXX_SYSINC += -isystem $(TP_GLIBC_INC_B)
endif

PROJECT_INCLUDE_DIRS := $(shell find Kernel -type d) $(shell find Library -type d)
INCLUDES := $(foreach d,$(PROJECT_INCLUDE_DIRS),-I$(d))

CPP_SOURCES := $(shell find Kernel -type f -name '*.cpp') \
               $(shell find Library -type f -name '*.cpp')
C_SOURCES   := $(shell find Kernel -type f -name '*.c') \
               $(shell find Library -type f -name '*.c')
ASM_SOURCES := $(shell find Kernel -type f -name '*.S')

INT_SRC  := Kernel/Arch/x86_64/CPU/Interrupt/InterruptDispatch.cpp
CPP_SOURCES := $(filter-out $(INT_SRC),$(CPP_SOURCES))

OBJ_CPP  := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SOURCES))
OBJ_C    := $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
OBJ_S    := $(patsubst %.S,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
INT_OBJ  := $(OBJ_DIR)/$(INT_SRC:.cpp=.o)
ALL_OBJS := $(OBJ_CPP) $(OBJ_C) $(OBJ_S) $(INT_OBJ)

DEPS := $(ALL_OBJS:.o=.d)

.PHONY: all clean
all: $(KERNEL_ELF)

$(KERNEL_ELF): $(ALL_OBJS) $(LDSCRIPT)
	@echo "LD   $@"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(ALL_OBJS) -lgcc

$(OBJ_DIR)/%.o: %.cpp
	@echo "CXX  $<"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(STDCXX_SYSINC) $(INCLUDES) -MMD -MP -c $< -o $@

$(INT_OBJ): $(INT_SRC)
	@echo "CXX-INT $<"
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_INTERRUPT) $(STDCXX_SYSINC) $(INCLUDES) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@echo "CC   $<"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(OBJ_DIR)/%.o: %.S
	@echo "AS   $<"
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPS)

clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)