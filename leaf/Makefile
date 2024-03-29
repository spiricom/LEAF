######################################
# stm32 makefile copied from DaisySP Makefile
#####################################

TARGET = libleaf

MODULE_DIR=src


######################################
# building variables
######################################
#DEBUG = 0
OPT = -O3

#######################################
# paths
#######################################

# Build path
BUILD_DIR = build

######################################
# source
######################################

# manually adding necessary HAL files
# generated by dump_filepath.py
C_SOURCES = 
C_SOURCES += \
Src/leaf-math.c \
Src/leaf-mempool.c \
Src/leaf-tables.c \
Src/leaf-distortion.c \
Src/leaf-dynamics.c \
Src/leaf-analysis.c \
Src/leaf-delay.c \
Src/leaf-oscillators.c \
Src/leaf-effects.c \
Src/leaf-electrical.c \
Src/leaf-envelopes.c \
Src/leaf-filters.c \
Src/leaf-instruments.c \
Src/leaf-reverb.c \
Src/leaf-midi.c \
Src/leaf-physical.c \
Src/leaf-sampling.c \
leaf.c \
Externals/d_fft_mayer.c






#STARTUP_PATH = Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc
# ASM sources
#ASM_SOURCES =  \
#$(STARTUP_PATH)/startup_stm32h750xx.s

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CXX = $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
AR = $(GCC_PATH)/$(PREFIX)ar
GDB = $(GCC_PATH)/$(PREFIX)gdb
else
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
AR = $(PREFIX)ar
GDB = $(PREFIX)gdb
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m7

# fpu
FPU = -mfpu=fpv5-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = -mthumb $(FLOAT-ABI) $(FPU) $(CPU)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DCORE_CM7  \
-DSTM32H750xx \
-DSTM32H750IB \
-Dflash_layout \
-DHSE_VALUE=16000000 \
-DUSE_HAL_DRIVER \
-DUSE_FULL_LL_DRIVER \
-DDATA_IN_D2_SRAM 
# ^ added for easy startup access


C_INCLUDES = \
-I$(MODULE_DIR) \
-IInc/ \
-IExternals \
-I. \

# suppressions for warnings introduced by HAL/FatFS
WARNINGS += -Wall -Wno-attributes -Wno-strict-aliasing -Wno-maybe-uninitialized -Wno-missing-attributes -Wno-stringop-overflow #-Werror
CPP_WARNINGS += -Wno-register

# compile gcc flags
ASFLAGS = $(MCU) $(AS_INCLUDES) $(AS_DEFS) -ggdb $(WARNINGS) $(OPT) -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_INCLUDES) $(C_DEFS) -ggdb $(WARNINGS) $(OPT) -fasm -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -ggdb
OPT = -O0
C_DEFS += -DDEBUG=1
else
C_DEFS += -DNDEBUG=1 -DRELEASE=1
endif

CFLAGS += \
-finline-functions

# C++ Flags
CPPFLAGS = $(CFLAGS) $(CPP_WARNINGS)
CPPFLAGS += \
-fno-exceptions \
-fno-rtti 

C_STANDARD = -std=gnu11
CPP_STANDARD += -std=gnu++14

# default action: build all
all: $(BUILD_DIR)/$(TARGET).a

#######################################
# build the application
#######################################

# list of C program objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of CPP program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(CPP_SOURCES:.cpp=.o))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# Prunes duplicates, and orders lexically (for archive build)
SORTED_OBJECTS = $(sort $(OBJECTS))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(C_STANDARD) -c $< -o $@ -MD -MP -MF $(BUILD_DIR)/$(notdir $(<:.c=.dep))

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CPP_STANDARD) -c $< -o $@ -MD -MP -MF $(BUILD_DIR)/$(notdir $(<:.cpp=.dep))

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	mkdir -p $(@D)
	$(AS) -c $(ASFLAGS) $< -o $@ -MD -MP -MF $(BUILD_DIR)/$(notdir $(<:.s =.dep))

$(BUILD_DIR)/$(TARGET).a: $(SORTED_OBJECTS) Makefile
	$(AR) -r $@ $(SORTED_OBJECTS)

$(BUILD_DIR):
	mkdir $@        

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
#######################################

# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.dep)
