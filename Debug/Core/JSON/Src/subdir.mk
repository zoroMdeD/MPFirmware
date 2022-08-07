################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/JSON/Src/cJSON.c \
../Core/JSON/Src/create_JSON.c \
../Core/JSON/Src/input_JSON.c 

OBJS += \
./Core/JSON/Src/cJSON.o \
./Core/JSON/Src/create_JSON.o \
./Core/JSON/Src/input_JSON.o 

C_DEPS += \
./Core/JSON/Src/cJSON.d \
./Core/JSON/Src/create_JSON.d \
./Core/JSON/Src/input_JSON.d 


# Each subdirectory must supply rules for building sources it contributes
Core/JSON/Src/%.o Core/JSON/Src/%.su: ../Core/JSON/Src/%.c Core/JSON/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-JSON-2f-Src

clean-Core-2f-JSON-2f-Src:
	-$(RM) ./Core/JSON/Src/cJSON.d ./Core/JSON/Src/cJSON.o ./Core/JSON/Src/cJSON.su ./Core/JSON/Src/create_JSON.d ./Core/JSON/Src/create_JSON.o ./Core/JSON/Src/create_JSON.su ./Core/JSON/Src/input_JSON.d ./Core/JSON/Src/input_JSON.o ./Core/JSON/Src/input_JSON.su

.PHONY: clean-Core-2f-JSON-2f-Src

