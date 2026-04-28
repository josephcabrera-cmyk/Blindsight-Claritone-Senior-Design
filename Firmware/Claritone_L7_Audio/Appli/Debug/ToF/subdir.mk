################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ToF/claritone_tof_test.c \
../ToF/platform.c \
../ToF/tof_front.c 

OBJS += \
./ToF/claritone_tof_test.o \
./ToF/platform.o \
./ToF/tof_front.o 

C_DEPS += \
./ToF/claritone_tof_test.d \
./ToF/platform.d \
./ToF/tof_front.d 


# Each subdirectory must supply rules for building sources it contributes
ToF/%.o ToF/%.su ToF/%.cyclo: ../ToF/%.c ToF/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I"C:/Users/Thepr/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L7_Audio/Appli/Audio" -I"C:/Users/Thepr/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L7_Audio/Appli/ToF" -I"C:/Users/Thepr/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L7_Audio/Appli/Drivers/VL53L7CX_ULD/Inc" -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ToF

clean-ToF:
	-$(RM) ./ToF/claritone_tof_test.cyclo ./ToF/claritone_tof_test.d ./ToF/claritone_tof_test.o ./ToF/claritone_tof_test.su ./ToF/platform.cyclo ./ToF/platform.d ./ToF/platform.o ./ToF/platform.su ./ToF/tof_front.cyclo ./ToF/tof_front.d ./ToF/tof_front.o ./ToF/tof_front.su

.PHONY: clean-ToF

