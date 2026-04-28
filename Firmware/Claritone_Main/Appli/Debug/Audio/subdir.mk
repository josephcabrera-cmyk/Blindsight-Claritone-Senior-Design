################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Audio/claritone_audio.c 

OBJS += \
./Audio/claritone_audio.o 

C_DEPS += \
./Audio/claritone_audio.d 


# Each subdirectory must supply rules for building sources it contributes
Audio/%.o Audio/%.su Audio/%.cyclo: ../Audio/%.c Audio/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L8_Integration/Appli/Drivers/VL53L7CX_ULD/Inc" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_Main/Appli/Drivers/VL53L7CX_ULD" -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_Main/Appli/ToF" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Audio

clean-Audio:
	-$(RM) ./Audio/claritone_audio.cyclo ./Audio/claritone_audio.d ./Audio/claritone_audio.o ./Audio/claritone_audio.su

.PHONY: clean-Audio

