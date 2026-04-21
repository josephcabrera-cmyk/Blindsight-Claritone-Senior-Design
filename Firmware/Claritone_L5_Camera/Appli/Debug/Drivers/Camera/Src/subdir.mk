################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Camera/Src/isp_ae_algo.c \
../Drivers/Camera/Src/isp_algo.c \
../Drivers/Camera/Src/isp_awb_algo.c \
../Drivers/Camera/Src/isp_cmd_parser.c \
../Drivers/Camera/Src/isp_conf.c \
../Drivers/Camera/Src/isp_core.c \
../Drivers/Camera/Src/isp_services.c \
../Drivers/Camera/Src/isp_tool_com.c 

OBJS += \
./Drivers/Camera/Src/isp_ae_algo.o \
./Drivers/Camera/Src/isp_algo.o \
./Drivers/Camera/Src/isp_awb_algo.o \
./Drivers/Camera/Src/isp_cmd_parser.o \
./Drivers/Camera/Src/isp_conf.o \
./Drivers/Camera/Src/isp_core.o \
./Drivers/Camera/Src/isp_services.o \
./Drivers/Camera/Src/isp_tool_com.o 

C_DEPS += \
./Drivers/Camera/Src/isp_ae_algo.d \
./Drivers/Camera/Src/isp_algo.d \
./Drivers/Camera/Src/isp_awb_algo.d \
./Drivers/Camera/Src/isp_cmd_parser.d \
./Drivers/Camera/Src/isp_conf.d \
./Drivers/Camera/Src/isp_core.d \
./Drivers/Camera/Src/isp_services.d \
./Drivers/Camera/Src/isp_tool_com.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Camera/Src/%.o Drivers/Camera/Src/%.su Drivers/Camera/Src/%.cyclo: ../Drivers/Camera/Src/%.c Drivers/Camera/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Secure_nsclib -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera/Inc" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera/IMX335" -I"C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Appli/Drivers/Camera/isp_param_conf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Camera-2f-Src

clean-Drivers-2f-Camera-2f-Src:
	-$(RM) ./Drivers/Camera/Src/isp_ae_algo.cyclo ./Drivers/Camera/Src/isp_ae_algo.d ./Drivers/Camera/Src/isp_ae_algo.o ./Drivers/Camera/Src/isp_ae_algo.su ./Drivers/Camera/Src/isp_algo.cyclo ./Drivers/Camera/Src/isp_algo.d ./Drivers/Camera/Src/isp_algo.o ./Drivers/Camera/Src/isp_algo.su ./Drivers/Camera/Src/isp_awb_algo.cyclo ./Drivers/Camera/Src/isp_awb_algo.d ./Drivers/Camera/Src/isp_awb_algo.o ./Drivers/Camera/Src/isp_awb_algo.su ./Drivers/Camera/Src/isp_cmd_parser.cyclo ./Drivers/Camera/Src/isp_cmd_parser.d ./Drivers/Camera/Src/isp_cmd_parser.o ./Drivers/Camera/Src/isp_cmd_parser.su ./Drivers/Camera/Src/isp_conf.cyclo ./Drivers/Camera/Src/isp_conf.d ./Drivers/Camera/Src/isp_conf.o ./Drivers/Camera/Src/isp_conf.su ./Drivers/Camera/Src/isp_core.cyclo ./Drivers/Camera/Src/isp_core.d ./Drivers/Camera/Src/isp_core.o ./Drivers/Camera/Src/isp_core.su ./Drivers/Camera/Src/isp_services.cyclo ./Drivers/Camera/Src/isp_services.d ./Drivers/Camera/Src/isp_services.o ./Drivers/Camera/Src/isp_services.su ./Drivers/Camera/Src/isp_tool_com.cyclo ./Drivers/Camera/Src/isp_tool_com.d ./Drivers/Camera/Src/isp_tool_com.o ./Drivers/Camera/Src/isp_tool_com.su

.PHONY: clean-Drivers-2f-Camera-2f-Src

