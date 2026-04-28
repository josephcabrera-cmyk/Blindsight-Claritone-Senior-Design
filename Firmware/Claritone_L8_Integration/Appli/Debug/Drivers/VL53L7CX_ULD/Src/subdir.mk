################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/VL53L7CX_ULD/Src/vl53l7cx_api.c 

OBJS += \
./Drivers/VL53L7CX_ULD/Src/vl53l7cx_api.o 

C_DEPS += \
./Drivers/VL53L7CX_ULD/Src/vl53l7cx_api.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/VL53L7CX_ULD/Src/%.o Drivers/VL53L7CX_ULD/Src/%.su Drivers/VL53L7CX_ULD/Src/%.cyclo: ../Drivers/VL53L7CX_ULD/Src/%.c Drivers/VL53L7CX_ULD/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L8_Integration/Appli/Drivers/VL53L7CX_ULD/Inc" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L8_Integration/Appli/Drivers/VL53L7CX_ULD" -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L8_Integration/Appli/ToF" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-VL53L7CX_ULD-2f-Src

clean-Drivers-2f-VL53L7CX_ULD-2f-Src:
	-$(RM) ./Drivers/VL53L7CX_ULD/Src/vl53l7cx_api.cyclo ./Drivers/VL53L7CX_ULD/Src/vl53l7cx_api.d ./Drivers/VL53L7CX_ULD/Src/vl53l7cx_api.o ./Drivers/VL53L7CX_ULD/Src/vl53l7cx_api.su

.PHONY: clean-Drivers-2f-VL53L7CX_ULD-2f-Src

