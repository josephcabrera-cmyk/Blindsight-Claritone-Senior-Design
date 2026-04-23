######################################
# quiet mode
######################################
V = 0
ifeq ($(V), 0)
  quiet = quiet_
else
  quiet =
endif
quiet_CC  = @echo "  CC $@"; $(CC)
quiet_LD  = @echo "  LD $@"; $(CC)
quiet_AS  = @echo "  AS $@"; $(AS)
quiet_SZ  = @echo "  SZ $@"; $(SZ)
quiet_BIN = @echo "  BIN $@"; $(BIN)

######################################
# helpers
######################################
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

######################################
# target
######################################
TARGET = Project
MODEL_DIR = ../../Model/CLARITONE-CUSTOM-PCB
BINARY_DIR = Binary
ifdef APP_GIT_SHA1_STRING
C_DEFS += -DAPP_GIT_SHA1_STRING=\"$(APP_GIT_SHA1_STRING)\"
endif
ifdef APP_VERSION_STRING
C_DEFS += -DAPP_VERSION_STRING=\"$(APP_VERSION_STRING)\"
endif

######################################
# building variables
######################################
OPT = -Os -g3

#######################################
# paths
#######################################
BUILD_TOP = build
BUILD_DIR = $(BUILD_TOP)/Application/CLARITONE-CUSTOM-PCB

######################################
# source
######################################
C_SOURCES += Src/main.c
C_SOURCES += Src/syscalls.c
C_SOURCES += Src/stm32n6xx_it.c
C_SOURCES += Src/crop_img.c
C_SOURCES += Src/app_camerapipeline.c
C_SOURCES += $(wildcard ../../Middlewares/ai-postprocessing-wrapper/*.c)
C_SOURCES += $(wildcard ../../Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Src/*.c)
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/Devices/STM32N6xx/mcu_cache.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/Devices/STM32N6xx/npu_cache.c
C_SOURCES += $(MODEL_DIR)/network.c
C_SOURCES += $(MODEL_DIR)/stai_network.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/CMSIS/Device/ST/STM32N6xx/Source/Templates/system_stm32n6xx_s.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_cacheaxi.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_cortex.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dcmipp.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma_ex.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_gpio.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_i2c.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_i2c_ex.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr_ex.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_ramcfg.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc_ex.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rif.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_uart.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/cmw_camera.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/cmw_utils.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/BSP/STM32N6xx_Nucleo/stm32n6xx_nucleo.c
C_SOURCES += ../../STM32Cube_FW_N6/Drivers/BSP/STM32N6xx_Nucleo/stm32n6xx_nucleo_bus.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/sensors/vd55g1/vd55g1.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/sensors/cmw_vd55g1.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/ISP_Library/isp/Src/isp_algo.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/ISP_Library/isp/Src/isp_awb_algo.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/ISP_Library/isp/Src/isp_ae_algo.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/ISP_Library/isp/Src/isp_cmd_parser.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/ISP_Library/isp/Src/isp_core.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/ISP_Library/isp/Src/isp_services.c
C_SOURCES += ../../Middlewares/stm32-mw-camera/ISP_Library/isp/Src/isp_tool_com.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_rt_main.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_runtime.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_cipher.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_stai_internal.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_util.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_float.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_sw_integer.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib.c
C_SOURCES += ../../Middlewares/stedgeai-lib/Npu/ll_aton/ll_aton_lib_sw_operators.c

ASM_SOURCES += ../../STM32Cube_FW_N6/Drivers/CMSIS/Device/ST/STM32N6xx/Source/Templates/gcc/startup_stm32n657xx.s

#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
READELF = $(GCC_PATH)/$(PREFIX)readelf
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
READELF = $(PREFIX)readelf
endif
LD = $(CC)
BIN = $(CP) -O binary

#######################################
# CFLAGS
#######################################
CPU = -mcpu=cortex-m55 -mcmse -mthumb
FPU = -mfpu=fpv5-d16 -mfloat-abi=hard
MCU = $(CPU) $(FPU)

C_DEFS += -DSTM32N657xx
C_DEFS += -DUSE_FULL_ASSERT
C_DEFS += -DUSE_FULL_LL_DRIVER
C_DEFS += -DVECT_TAB_SRAM
C_DEFS += -DLL_ATON_DUMP_DEBUG_API
C_DEFS += -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6
C_DEFS += -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL
C_DEFS += -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC
C_DEFS += -DLL_ATON_SW_FALLBACK
C_DEFS += -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1

C_INCLUDES += -IInc
C_INCLUDES += -I$(MODEL_DIR)
C_INCLUDES += -I../../Middlewares/ai-postprocessing-wrapper
C_INCLUDES += -I../../Middlewares/stm32-vision-models-postprocessing/lib_vision_models_pp/Inc
C_INCLUDES += -I../../Middlewares/stedgeai-lib/Npu/ll_aton
C_INCLUDES += -I../../Middlewares/stedgeai-lib/Npu/Devices/STM32N6xx
C_INCLUDES += -I../../Middlewares/stedgeai-lib/Inc
C_INCLUDES += -I../../Middlewares/stm32-mw-camera
C_INCLUDES += -I../../Middlewares/stm32-mw-camera/sensors
C_INCLUDES += -I../../Middlewares/stm32-mw-camera/sensors/vd55g1
C_INCLUDES += -I../../Middlewares/stm32-mw-camera/ISP_Library/isp/Inc
C_INCLUDES += -I../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc
C_INCLUDES += -I../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc/Legacy
C_INCLUDES += -I../../STM32Cube_FW_N6/Drivers/CMSIS/Device/ST/STM32N6xx/Include
C_INCLUDES += -I../../STM32Cube_FW_N6/Drivers/CMSIS/Include
C_INCLUDES += -I../../STM32Cube_FW_N6/Drivers/CMSIS/DSP/Include
C_INCLUDES += -I../../STM32Cube_FW_N6/Drivers/BSP/Components/Common
C_INCLUDES += -I../../STM32Cube_FW_N6/Drivers/BSP/STM32N6xx_Nucleo

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fstack-usage -fdata-sections -ffunction-sections -fcyclomatic-complexity
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fstack-usage -fdata-sections -ffunction-sections -fcyclomatic-complexity
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CFLAGS += -std=gnu11

#######################################
# LDFLAGS
#######################################
LDSCRIPT = STM32CubeIDE/STM32N657xx.ld
LIBS = -lc -lm -lnosys -l:NetworkRuntime1200_CM55_GCC.a
LIBDIR = -L../../Middlewares/stedgeai-lib/Lib/GCC/ARMCortexM55
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
LDFLAGS += -u _printf_float
LDFLAGS += -Wl,--print-memory-usage
LDFLAGS += -Wl,--no-warn-rwx-segments

#######################################
# base build
#######################################
.PHONY: all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

OBJECTS = $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES_S:.S=.o))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$($(quiet)CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$($(quiet)CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$($(quiet)AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).list: $(OBJECTS)
	$(file > $@, $(OBJECTS))

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile $(BUILD_DIR)/$(TARGET).list
	$($(quiet)LD) @$(BUILD_DIR)/$(TARGET).list $(LDFLAGS) -o $@
	$($(quiet)SZ) $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	-rm -fR $(BUILD_TOP)

-include $(call rwildcard,$(BUILD_TOP),*.d)
