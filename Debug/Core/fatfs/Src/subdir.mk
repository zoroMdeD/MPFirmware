################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/fatfs/Src/ccsbcs.c \
../Core/fatfs/Src/ff.c \
../Core/fatfs/Src/sd_cmd.c \
../Core/fatfs/Src/sd_main.c \
../Core/fatfs/Src/spi_sd.c 

OBJS += \
./Core/fatfs/Src/ccsbcs.o \
./Core/fatfs/Src/ff.o \
./Core/fatfs/Src/sd_cmd.o \
./Core/fatfs/Src/sd_main.o \
./Core/fatfs/Src/spi_sd.o 

C_DEPS += \
./Core/fatfs/Src/ccsbcs.d \
./Core/fatfs/Src/ff.d \
./Core/fatfs/Src/sd_cmd.d \
./Core/fatfs/Src/sd_main.d \
./Core/fatfs/Src/spi_sd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/fatfs/Src/%.o Core/fatfs/Src/%.su: ../Core/fatfs/Src/%.c Core/fatfs/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-fatfs-2f-Src

clean-Core-2f-fatfs-2f-Src:
	-$(RM) ./Core/fatfs/Src/ccsbcs.d ./Core/fatfs/Src/ccsbcs.o ./Core/fatfs/Src/ccsbcs.su ./Core/fatfs/Src/ff.d ./Core/fatfs/Src/ff.o ./Core/fatfs/Src/ff.su ./Core/fatfs/Src/sd_cmd.d ./Core/fatfs/Src/sd_cmd.o ./Core/fatfs/Src/sd_cmd.su ./Core/fatfs/Src/sd_main.d ./Core/fatfs/Src/sd_main.o ./Core/fatfs/Src/sd_main.su ./Core/fatfs/Src/spi_sd.d ./Core/fatfs/Src/spi_sd.o ./Core/fatfs/Src/spi_sd.su

.PHONY: clean-Core-2f-fatfs-2f-Src

