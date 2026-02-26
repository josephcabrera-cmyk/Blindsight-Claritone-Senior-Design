################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/main.c \
C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/stm32n6xx_hal_msp.c \
C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/stm32n6xx_it.c \
../Src/syscalls.c \
../Src/sysmem.c \
C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/system_stm32n6xx_s.c 

OBJS += \
./Src/main.o \
./Src/stm32n6xx_hal_msp.o \
./Src/stm32n6xx_it.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_stm32n6xx_s.o 

C_DEPS += \
./Src/main.d \
./Src/stm32n6xx_hal_msp.d \
./Src/stm32n6xx_it.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_stm32n6xx_s.d 


# Each subdirectory must supply rules for building sources it contributes
Src/main.o: C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/main.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DSTM32N6xx -DSTM32 -DSTM32N657xx -c -I../../../Appli/Inc -I../../../../../../../Drivers/BSP/STM32N6xx_Nucleo -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../../../../Drivers/STM32N6xx_HAL_Driver/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_msp.o: C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/stm32n6xx_hal_msp.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DSTM32N6xx -DSTM32 -DSTM32N657xx -c -I../../../Appli/Inc -I../../../../../../../Drivers/BSP/STM32N6xx_Nucleo -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../../../../Drivers/STM32N6xx_HAL_Driver/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_it.o: C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/stm32n6xx_it.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DSTM32N6xx -DSTM32 -DSTM32N657xx -c -I../../../Appli/Inc -I../../../../../../../Drivers/BSP/STM32N6xx_Nucleo -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../../../../Drivers/STM32N6xx_HAL_Driver/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DSTM32N6xx -DSTM32 -DSTM32N657xx -c -I../../../Appli/Inc -I../../../../../../../Drivers/BSP/STM32N6xx_Nucleo -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../../../../Drivers/STM32N6xx_HAL_Driver/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/system_stm32n6xx_s.o: C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Projects/NUCLEO-N657X0-Q/Templates/Template_FSBL_XIP/Appli/Src/system_stm32n6xx_s.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DSTM32N6xx -DSTM32 -DSTM32N657xx -c -I../../../Appli/Inc -I../../../../../../../Drivers/BSP/STM32N6xx_Nucleo -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../../../../Drivers/STM32N6xx_HAL_Driver/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/stm32n6xx_hal_msp.cyclo ./Src/stm32n6xx_hal_msp.d ./Src/stm32n6xx_hal_msp.o ./Src/stm32n6xx_hal_msp.su ./Src/stm32n6xx_it.cyclo ./Src/stm32n6xx_it.d ./Src/stm32n6xx_it.o ./Src/stm32n6xx_it.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/system_stm32n6xx_s.cyclo ./Src/system_stm32n6xx_s.d ./Src/system_stm32n6xx_s.o ./Src/system_stm32n6xx_s.su

.PHONY: clean-Src

