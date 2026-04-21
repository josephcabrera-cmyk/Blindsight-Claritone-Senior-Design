################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_cortex.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma_ex.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_exti.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_gpio.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr_ex.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc_ex.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rif.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_uart.c \
C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_uart_ex.c 

OBJS += \
./Src/stm32n6xx_hal.o \
./Src/stm32n6xx_hal_cortex.o \
./Src/stm32n6xx_hal_dma.o \
./Src/stm32n6xx_hal_dma_ex.o \
./Src/stm32n6xx_hal_exti.o \
./Src/stm32n6xx_hal_gpio.o \
./Src/stm32n6xx_hal_pwr.o \
./Src/stm32n6xx_hal_pwr_ex.o \
./Src/stm32n6xx_hal_rcc.o \
./Src/stm32n6xx_hal_rcc_ex.o \
./Src/stm32n6xx_hal_rif.o \
./Src/stm32n6xx_hal_uart.o \
./Src/stm32n6xx_hal_uart_ex.o 

C_DEPS += \
./Src/stm32n6xx_hal.d \
./Src/stm32n6xx_hal_cortex.d \
./Src/stm32n6xx_hal_dma.d \
./Src/stm32n6xx_hal_dma_ex.d \
./Src/stm32n6xx_hal_exti.d \
./Src/stm32n6xx_hal_gpio.d \
./Src/stm32n6xx_hal_pwr.d \
./Src/stm32n6xx_hal_pwr_ex.d \
./Src/stm32n6xx_hal_rcc.d \
./Src/stm32n6xx_hal_rcc_ex.d \
./Src/stm32n6xx_hal_rif.d \
./Src/stm32n6xx_hal_uart.d \
./Src/stm32n6xx_hal_uart_ex.d 


# Each subdirectory must supply rules for building sources it contributes
Src/stm32n6xx_hal.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_cortex.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_cortex.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_dma.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_dma_ex.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_dma_ex.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_exti.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_exti.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_gpio.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_gpio.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_pwr.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_pwr_ex.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_pwr_ex.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_rcc.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_rcc_ex.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rcc_ex.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_rif.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_rif.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_uart.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_uart.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32n6xx_hal_uart_ex.o: C:/Users/Thepr/STM32Projects/Blindsight-Claritone-Senior-Design/Firmware/Claritone_L5_Camera/Drivers/STM32N6xx_HAL_Driver/Src/stm32n6xx_hal_uart_ex.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -c -I../Core/Inc -I../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/stm32n6xx_hal.cyclo ./Src/stm32n6xx_hal.d ./Src/stm32n6xx_hal.o ./Src/stm32n6xx_hal.su ./Src/stm32n6xx_hal_cortex.cyclo ./Src/stm32n6xx_hal_cortex.d ./Src/stm32n6xx_hal_cortex.o ./Src/stm32n6xx_hal_cortex.su ./Src/stm32n6xx_hal_dma.cyclo ./Src/stm32n6xx_hal_dma.d ./Src/stm32n6xx_hal_dma.o ./Src/stm32n6xx_hal_dma.su ./Src/stm32n6xx_hal_dma_ex.cyclo ./Src/stm32n6xx_hal_dma_ex.d ./Src/stm32n6xx_hal_dma_ex.o ./Src/stm32n6xx_hal_dma_ex.su ./Src/stm32n6xx_hal_exti.cyclo ./Src/stm32n6xx_hal_exti.d ./Src/stm32n6xx_hal_exti.o ./Src/stm32n6xx_hal_exti.su ./Src/stm32n6xx_hal_gpio.cyclo ./Src/stm32n6xx_hal_gpio.d ./Src/stm32n6xx_hal_gpio.o ./Src/stm32n6xx_hal_gpio.su ./Src/stm32n6xx_hal_pwr.cyclo ./Src/stm32n6xx_hal_pwr.d ./Src/stm32n6xx_hal_pwr.o ./Src/stm32n6xx_hal_pwr.su ./Src/stm32n6xx_hal_pwr_ex.cyclo ./Src/stm32n6xx_hal_pwr_ex.d ./Src/stm32n6xx_hal_pwr_ex.o ./Src/stm32n6xx_hal_pwr_ex.su ./Src/stm32n6xx_hal_rcc.cyclo ./Src/stm32n6xx_hal_rcc.d ./Src/stm32n6xx_hal_rcc.o ./Src/stm32n6xx_hal_rcc.su ./Src/stm32n6xx_hal_rcc_ex.cyclo ./Src/stm32n6xx_hal_rcc_ex.d ./Src/stm32n6xx_hal_rcc_ex.o ./Src/stm32n6xx_hal_rcc_ex.su ./Src/stm32n6xx_hal_rif.cyclo ./Src/stm32n6xx_hal_rif.d ./Src/stm32n6xx_hal_rif.o ./Src/stm32n6xx_hal_rif.su ./Src/stm32n6xx_hal_uart.cyclo ./Src/stm32n6xx_hal_uart.d ./Src/stm32n6xx_hal_uart.o ./Src/stm32n6xx_hal_uart.su ./Src/stm32n6xx_hal_uart_ex.cyclo ./Src/stm32n6xx_hal_uart_ex.d ./Src/stm32n6xx_hal_uart_ex.o ./Src/stm32n6xx_hal_uart_ex.su

.PHONY: clean-Src

