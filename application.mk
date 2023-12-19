# Add path of toolchain to system path first, or enable ARM_GCC_TOOLCHAIN 
# -------------------------------------------------------------------
#ARM_GCC_TOOLCHAIN = toolchain/gcc-arm-none-eabi-4_9-2015q1/bin/

ARM_GCC_TOOLCHAIN = ${FREERTOS_EXEC_PATH}
CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)arm-none-eabi-

# Compilation tools
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

Q := @
ifeq ($(V),1)
Q := 
endif
OS := $(shell uname)
CFG_MBEDTLS ?= 0


ENCRYPT_ARGS = 
ifeq ($(findstring MINGW32_NT, $(OS)), MINGW32_NT) 
ENCRYPT = "./tools/crc_binary/encrypt.exe"
else ifeq ($(findstring CYGWIN, $(OS)), CYGWIN) 
ENCRYPT = "./tools/crc_binary/encrypt.exe"
else ifeq ($(findstring Darwin, $(OS)), Darwin) 
ENCRYPT = "./tools/crc_binary/encrypt.darwin"
ENCRYPT_ARGS = 0 0 0
else
ENCRYPT = "./tools/crc_binary/encrypt"
ENCRYPT_ARGS = 0 0 0
endif

ifeq ($(ECHO),)
ECHO=echo
endif

-include .config

# -------------------------------------------------------------------
# Initialize target name and target object files
# -------------------------------------------------------------------

all: application 

TARGET=out

OBJ_DIR=$(TARGET)
BIN_DIR=$(TARGET)

BEKEN_DIR ?= .

# -------------------------------------------------------------------
# Include folder list
# -------------------------------------------------------------------
INCLUDES =
INCLUDES += -I./config
INCLUDES += -I./release
INCLUDES += -I./os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/Keil/ARM968es
INCLUDES += -I./os/FreeRTOSv9.0.0/FreeRTOS/Source/include
INCLUDES += -I$(BEKEN_DIR)/include
INCLUDES += -I$(BEKEN_DIR)/app/config
INCLUDES += -I$(BEKEN_DIR)/driver/entry
INCLUDES += -I$(BEKEN_DIR)/driver/intc
INCLUDES += -I$(BEKEN_DIR)/driver/pwm
INCLUDES += -I$(BEKEN_DIR)/driver/flash
INCLUDES += -I$(BEKEN_DIR)/driver/common
INCLUDES += -I$(BEKEN_DIR)/driver/uart
INCLUDES += -I$(BEKEN_DIR)/driver/sys_ctrl
INCLUDES += -I$(BEKEN_DIR)/driver/gpio
INCLUDES += -I$(BEKEN_DIR)/driver/icu
INCLUDES += -I$(BEKEN_DIR)/components/user_driver
INCLUDES += -I$(BEKEN_DIR)/components/uart_debug
INCLUDES += -I$(BEKEN_DIR)/os/include
INCLUDES += -I$(BEKEN_DIR)/os/FreeRTOSv9.0.0
INCLUDES += -I$(BEKEN_DIR)/components/utf8
INCLUDES += -I$(BEKEN_DIR)/app/http

# CherryUSB
ifeq ($(CFG_CHERRY_USB),1)
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/common
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/core
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/osal
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/cdc
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/hid
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/msc
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/audio
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/video
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/class/hub
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/demo
INCLUDES += -I$(BEKEN_DIR)/components/CherryUSB/demo/beken/usb_device
ifeq ($(CFG_UF2),1)
INCLUDES += -I$(BEKEN_DIR)/components/uf2
INCLUDES += -I$(BEKEN_DIR)/components/uf2/ports
endif
endif



# -------------------------------------------------------------------
# Source file list
# -------------------------------------------------------------------
SRC_C =
SRC_OS =
SRC_S =

#SRC_OS += ./os/FreeRTOSv9.0.0/FreeRTOS/Source/portable/Keil/ARM968es/port.c
SRC_C += $(BEKEN_DIR)/driver/common/dd.c
SRC_C += $(BEKEN_DIR)/driver/common/drv_model.c
SRC_C += $(BEKEN_DIR)/driver/driver.c
SRC_C += $(BEKEN_DIR)/driver/entry/arch_main.c
SRC_C += $(BEKEN_DIR)/driver/flash/flash.c
SRC_C += $(BEKEN_DIR)/driver/gpio/gpio.c
SRC_C += $(BEKEN_DIR)/driver/icu/icu.c
SRC_C += $(BEKEN_DIR)/driver/intc/intc.c
SRC_C += $(BEKEN_DIR)/driver/pwm/bk_timer.c
SRC_C += $(BEKEN_DIR)/driver/sys_ctrl/sys_ctrl.c
SRC_C += $(BEKEN_DIR)/driver/uart/uart.c
SRC_C += $(BEKEN_DIR)/driver/uart/printf.c
SRC_C += $(BEKEN_DIR)/driver/wdt/wdt.c
SRC_C += $(BEKEN_DIR)/components/func.c
SRC_C += $(BEKEN_DIR)/components/misc/fake_clock.c
SRC_C += $(BEKEN_DIR)/components/misc/target_util.c
SRC_C += $(BEKEN_DIR)/components/misc/start_type.c
SRC_C += $(BEKEN_DIR)/components/user_driver/BkDriverWdg.c

# CherryUSB
ifeq ($(CFG_CHERRY_USB),1)
SRC_C += $(BEKEN_DIR)/components/CherryUSB/core/usbd_core.c
SRC_C += $(BEKEN_DIR)/components/CherryUSB/class/msc/usbd_msc.c
SRC_C += $(BEKEN_DIR)/components/CherryUSB/port/beken_musb/usb_dc_beken_musb.c

# UF2
ifeq ($(CFG_UF2),1)
SRC_C += $(BEKEN_DIR)/components/uf2/ghostfat.c
SRC_C += $(BEKEN_DIR)/components/uf2/msc_desc.c
SRC_C += $(BEKEN_DIR)/components/uf2/ports/board_flash.c
endif # CFG_UF2
endif

#assembling files
SRC_S +=  $(BEKEN_DIR)/driver/entry/boot_handlers.S
SRC_S +=  $(BEKEN_DIR)/driver/entry/boot_vectors.S


# Generate obj list
# -------------------------------------------------------------------
OBJ_LIST = $(SRC_C:%.c=$(OBJ_DIR)/%.o)
DEPENDENCY_LIST = $(SRC_C:%.c=$(OBJ_DIR)/%.d)

OBJ_S_LIST = $(SRC_S:%.S=$(OBJ_DIR)/%.O)
DEPENDENCY_S_LIST = $(SRC_S:%.S=$(OBJ_DIR)/%.d)

OBJ_OS_LIST = $(SRC_OS:%.c=$(OBJ_DIR)/%.marm.o)
DEPENDENCY_OS_LIST = $(SRC_OS:%.c=$(OBJ_DIR)/%.d)

ifeq ($(CFG_SOC_NAME), 1)
SOC_NAME_ELF = beken7231.elf
SOC_NAME_BIN = beken7231.bin
SOC_NAME_MAP = beken7231.map
SOC_NAME_LDS = bk7231.ld
SOC_NAME_BSP_LDS = bk7231_bsp.ld
else ifeq ($(CFG_SOC_NAME), 2)
SOC_NAME_ELF = beken7231u.elf
SOC_NAME_BIN = beken7231u.bin
SOC_NAME_MAP = beken7231u.map
SOC_NAME_LDS = bk7231.ld
SOC_NAME_BSP_LDS = bk7231_bsp.ld
else ifeq ($(CFG_SOC_NAME), 3)
SOC_NAME_ELF = beken7251.elf
SOC_NAME_BIN = beken7251.bin
SOC_NAME_MAP = beken7251.map
SOC_NAME_LDS = bk7231.ld
SOC_NAME_BSP_LDS = bk7231_bsp.ld
else ifeq ($(CFG_SOC_NAME), 5)
SOC_NAME_ELF = beken7231n.elf
SOC_NAME_BIN = beken7231n.bin
SOC_NAME_MAP = beken7231n.map
SOC_NAME_LDS = bk7231n.lds
SOC_NAME_BSP_LDS = bk7231n_bsp.lds
endif
SOC_NAME_BSP_ELF = beken7231_bsp.elf
SOC_NAME_BSP_BIN = beken7231_bsp.bin
SOC_NAME_BSP_MAP = beken7231_bsp.map

# Compile options
# -------------------------------------------------------------------
CFLAGS += -g -mthumb -mcpu=arm968e-s -march=armv5te -mthumb-interwork -mlittle-endian -Os -std=c99 -ffunction-sections -Wall -Wno-format -Wno-unknown-pragmas -fsigned-char -fdata-sections -nostdlib -fno-strict-aliasing
#CFLAGS += -g -mthumb -mcpu=arm968e-s -march=armv5te -mthumb-interwork -mlittle-endian -Os -std=c99 -ffunction-sections -Wall -Wno-unused-function -fsigned-char -fdata-sections -Wunknown-pragmas -nostdlib -Wl,--gc-sections

OSFLAGS += -g -marm -mcpu=arm968e-s -march=armv5te -mthumb-interwork -mlittle-endian -Os -std=c99 -ffunction-sections -Wall -fsigned-char -fdata-sections -Wunknown-pragmas
#OSFLAGS += -g -mthumb -mcpu=arm968e-s -march=armv5te -mthumb-interwork -mlittle-endian -Os -std=c99 -ffunction-sections -Wall -fsigned-char -fdata-sections -Wunknown-pragmas -Wl,--gc-sections

ASMFLAGS = 
ASMFLAGS += -g -marm -mthumb-interwork -mcpu=arm968e-s -march=armv5te -x assembler-with-cpp

LFLAGS = 
LFLAGS += -g -Wl,--gc-sections -marm -mcpu=arm968e-s -mthumb-interwork -nostdlib  -Xlinker -Map=beken.map
LFLAGS += -Wl,-wrap,printf -Wl,-wrap,vsnprintf -Wl,-wrap,snprintf -Wl,-wrap,sprintf -Wl,-wrap,puts
#LFLAGS += -g -Wl,--gc-sections -mthumb -mcpu=arm968e-s -mthumb-interwork -nostdlib


.PHONY: application
application: $(OBJ_LIST) $(OBJ_S_LIST) $(OBJ_OS_LIST)

	@$(ECHO) "  $(GREEN)LD   $(BIN_DIR)/$(SOC_NAME_ELF)$(NC)"
	$(Q)$(LD) $(LFLAGS) -o $(BIN_DIR)/$(SOC_NAME_ELF) $(OBJ_LIST) $(OBJ_S_LIST) $(OBJ_OS_LIST) $(LIBFLAGS) -T./build/$(SOC_NAME_LDS) -Xlinker -Map=$(BIN_DIR)/$(SOC_NAME_MAP)
	$(Q)$(OBJCOPY) -O binary $(BIN_DIR)/$(SOC_NAME_ELF) $(BIN_DIR)/$(SOC_NAME_BIN)
#	$(OBJDUMP) -d $(BIN_DIR)/$(SOC_NAME_ELF) >> $(BIN_DIR)/beken7231.asm
#	@$(ECHO) "                                                        "
	@$(ECHO) "  $(GREEN)CRC  $(BIN_DIR)/$(SOC_NAME_BIN)$(NC)"
	$(Q)$(ENCRYPT) $(BIN_DIR)/$(SOC_NAME_BIN) 0 $(ENCRYPT_ARGS) > /dev/null
	$(Q)$(LD) $(LFLAGS) -o $(BIN_DIR)/$(SOC_NAME_BSP_ELF) $(OBJ_LIST) $(OBJ_S_LIST) $(OBJ_OS_LIST) $(LIBFLAGS) -T./build/$(SOC_NAME_BSP_LDS) -Xlinker -Map=$(BIN_DIR)/$(SOC_NAME_BSP_MAP)
	$(Q)$(OBJCOPY) -O binary $(BIN_DIR)/$(SOC_NAME_BSP_ELF) $(BIN_DIR)/$(SOC_NAME_BSP_BIN)
#	$(Q)(cd ./tools/beken_packager; ./beken_packager_wrapper -i $(CFG_SOC_NAME))
	$(Q)$(ENCRYPT) $(BIN_DIR)/$(SOC_NAME_BSP_BIN) 0 $(ENCRYPT_ARGS) > /dev/null

$(OBJ_DIR)/%.o: %.c
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)$(ECHO) "  $(GREEN)CC   $<$(NC)"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(patsubst %.o,%.d,$@)

$(OBJ_DIR)/%.O: %.S
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)$(ECHO) "  $(GREEN)AS   $<$(NC)"
	$(Q)$(CC) $(ASMFLAGS) $(INCLUDES) -c $< -o $@
	$(Q)$(CC) $(ASMFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(patsubst %.O,%.d,$@)

$(OBJ_DIR)/%.marm.o: %.c
	$(Q)if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(Q)$(ECHO) "  $(GREEN)CC   $<$(NC)"
	$(Q)$(CC) $(OSFLAGS) $(INCLUDES) -c $< -o $@
	$(Q)$(CC) $(OSFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(patsubst %.marm.o,%.d,$@)

-include $(DEPENDENCY_LIST)
-include $(DEPENDENCY_S_LIST)
-include $(DEPENDENCY_OS_LIST)

.PHONY: clean
clean:
	$(Q)-rm -rf $(TARGET)
	$(Q)-rm -f .config
	@$(ECHO) "$(GREEN)Done$(NC)"

.PHONY: cleanlib
cleanlib:
	$(Q)-rm -rf $(RWNX_LIB) $(WPA_LIB) $(USB_LIB) $(BLE_LIB) $(SENSOR_LIB) $(CAL_LIB) $(SUPPLICANT_LIB) $(UART_DEBUG_LIB) $(RF_TEST_LIB) $(RF_USE_LIB)
	@$(ECHO) "$(GREEN)Done$(NC)"
