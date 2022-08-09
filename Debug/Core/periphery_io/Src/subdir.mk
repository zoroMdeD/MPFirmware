################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/periphery_io/Src/cmd.c \
../Core/periphery_io/Src/data_process.c 

OBJS += \
./Core/periphery_io/Src/cmd.o \
./Core/periphery_io/Src/data_process.o 

C_DEPS += \
./Core/periphery_io/Src/cmd.d \
./Core/periphery_io/Src/data_process.d 


# Each subdirectory must supply rules for building sources it contributes
Core/periphery_io/Src/%.o Core/periphery_io/Src/%.su: ../Core/periphery_io/Src/%.c Core/periphery_io/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-periphery_io-2f-Src

clean-Core-2f-periphery_io-2f-Src:
	-$(RM) ./Core/periphery_io/Src/cmd.d ./Core/periphery_io/Src/cmd.o ./Core/periphery_io/Src/cmd.su ./Core/periphery_io/Src/data_process.d ./Core/periphery_io/Src/data_process.o ./Core/periphery_io/Src/data_process.su

.PHONY: clean-Core-2f-periphery_io-2f-Src

