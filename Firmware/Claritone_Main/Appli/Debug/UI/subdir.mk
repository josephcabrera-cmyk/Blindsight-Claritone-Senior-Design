################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/claritone_ui.c 

OBJS += \
./UI/claritone_ui.o 

C_DEPS += \
./UI/claritone_ui.d 


# Each subdirectory must supply rules for building sources it contributes
UI/%.o UI/%.su UI/%.cyclo: ../UI/%.c UI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L8_Integration/Appli/Drivers/VL53L7CX_ULD/Inc" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_Main/Appli/Drivers/VL53L7CX_ULD" -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_Main/Appli/ToF" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI

clean-UI:
	-$(RM) ./UI/claritone_ui.cyclo ./UI/claritone_ui.d ./UI/claritone_ui.o ./UI/claritone_ui.su

.PHONY: clean-UI

