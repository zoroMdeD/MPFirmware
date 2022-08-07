################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/led/Src/fonts.c \
../Core/led/Src/led.c \
../Core/led/Src/ssd1306.c 

OBJS += \
./Core/led/Src/fonts.o \
./Core/led/Src/led.o \
./Core/led/Src/ssd1306.o 

C_DEPS += \
./Core/led/Src/fonts.d \
./Core/led/Src/led.d \
./Core/led/Src/ssd1306.d 


# Each subdirectory must supply rules for building sources it contributes
Core/led/Src/%.o Core/led/Src/%.su: ../Core/led/Src/%.c Core/led/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-led-2f-Src

clean-Core-2f-led-2f-Src:
	-$(RM) ./Core/led/Src/fonts.d ./Core/led/Src/fonts.o ./Core/led/Src/fonts.su ./Core/led/Src/led.d ./Core/led/Src/led.o ./Core/led/Src/led.su ./Core/led/Src/ssd1306.d ./Core/led/Src/ssd1306.o ./Core/led/Src/ssd1306.su

.PHONY: clean-Core-2f-led-2f-Src

