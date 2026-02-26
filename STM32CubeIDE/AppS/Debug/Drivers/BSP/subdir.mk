################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Drivers/BSP/STM32N6xx_Nucleo/stm32n6xx_nucleo.c 

OBJS += \
./Drivers/BSP/stm32n6xx_nucleo.o 

C_DEPS += \
./Drivers/BSP/stm32n6xx_nucleo.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/stm32n6xx_nucleo.o: C:/Users/zebes/Blindsight_Projects/STM32Cube_FW_N6_V1.3.0/Drivers/BSP/STM32N6xx_Nucleo/stm32n6xx_nucleo.c Drivers/BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g -DDEBUG -DSTM32N6xx -DSTM32 -DSTM32N657xx -c -I../../../Appli/Inc -I../../../../../../../Drivers/BSP/STM32N6xx_Nucleo -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../../../../Drivers/STM32N6xx_HAL_Driver/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP

clean-Drivers-2f-BSP:
	-$(RM) ./Drivers/BSP/stm32n6xx_nucleo.cyclo ./Drivers/BSP/stm32n6xx_nucleo.d ./Drivers/BSP/stm32n6xx_nucleo.o ./Drivers/BSP/stm32n6xx_nucleo.su

.PHONY: clean-Drivers-2f-BSP

