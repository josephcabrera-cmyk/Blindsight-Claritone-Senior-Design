################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Camera/IMX335/imx335.c \
../Drivers/Camera/IMX335/imx335_reg.c 

OBJS += \
./Drivers/Camera/IMX335/imx335.o \
./Drivers/Camera/IMX335/imx335_reg.o 

C_DEPS += \
./Drivers/Camera/IMX335/imx335.d \
./Drivers/Camera/IMX335/imx335_reg.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Camera/IMX335/%.o Drivers/Camera/IMX335/%.su Drivers/Camera/IMX335/%.cyclo: ../Drivers/Camera/IMX335/%.c Drivers/Camera/IMX335/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera/Inc" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera/IMX335" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera/isp_param_conf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Camera-2f-IMX335

clean-Drivers-2f-Camera-2f-IMX335:
	-$(RM) ./Drivers/Camera/IMX335/imx335.cyclo ./Drivers/Camera/IMX335/imx335.d ./Drivers/Camera/IMX335/imx335.o ./Drivers/Camera/IMX335/imx335.su ./Drivers/Camera/IMX335/imx335_reg.cyclo ./Drivers/Camera/IMX335/imx335_reg.d ./Drivers/Camera/IMX335/imx335_reg.o ./Drivers/Camera/IMX335/imx335_reg.su

.PHONY: clean-Drivers-2f-Camera-2f-IMX335

