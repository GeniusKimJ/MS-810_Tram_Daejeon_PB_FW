################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User_Src/AFE_adbms6815.c \
../User_Src/AFE_adbms6815_SM.c \
../User_Src/EEP_m24256a.c \
../User_Src/EEP_m24m02a.c \
../User_Src/FIRE_phsc220xcr.c \
../User_Src/GAS_sgxbld1.c \
../User_Src/HALL_cab1500.c \
../User_Src/HALL_s124.c \
../User_Src/HUMI_sht3xdis.c \
../User_Src/IMD_gqimdu2.c \
../User_Src/RTC_mcp7940a.c \
../User_Src/ms_Util.c \
../User_Src/ms_adc.c \
../User_Src/ms_alarm.c \
../User_Src/ms_boot.c \
../User_Src/ms_btms.c \
../User_Src/ms_can.c \
../User_Src/ms_can_mon.c \
../User_Src/ms_det.c \
../User_Src/ms_eep.c \
../User_Src/ms_flash.c \
../User_Src/ms_gpio.c \
../User_Src/ms_i2c.c \
../User_Src/ms_main.c \
../User_Src/ms_mvb.c \
../User_Src/ms_process.c \
../User_Src/ms_relay.c \
../User_Src/ms_rs232_mon.c \
../User_Src/ms_soc.c \
../User_Src/ms_spi.c \
../User_Src/ms_temp.c \
../User_Src/ms_timer.c \
../User_Src/ms_uart.c 

OBJS += \
./User_Src/AFE_adbms6815.o \
./User_Src/AFE_adbms6815_SM.o \
./User_Src/EEP_m24256a.o \
./User_Src/EEP_m24m02a.o \
./User_Src/FIRE_phsc220xcr.o \
./User_Src/GAS_sgxbld1.o \
./User_Src/HALL_cab1500.o \
./User_Src/HALL_s124.o \
./User_Src/HUMI_sht3xdis.o \
./User_Src/IMD_gqimdu2.o \
./User_Src/RTC_mcp7940a.o \
./User_Src/ms_Util.o \
./User_Src/ms_adc.o \
./User_Src/ms_alarm.o \
./User_Src/ms_boot.o \
./User_Src/ms_btms.o \
./User_Src/ms_can.o \
./User_Src/ms_can_mon.o \
./User_Src/ms_det.o \
./User_Src/ms_eep.o \
./User_Src/ms_flash.o \
./User_Src/ms_gpio.o \
./User_Src/ms_i2c.o \
./User_Src/ms_main.o \
./User_Src/ms_mvb.o \
./User_Src/ms_process.o \
./User_Src/ms_relay.o \
./User_Src/ms_rs232_mon.o \
./User_Src/ms_soc.o \
./User_Src/ms_spi.o \
./User_Src/ms_temp.o \
./User_Src/ms_timer.o \
./User_Src/ms_uart.o 

C_DEPS += \
./User_Src/AFE_adbms6815.d \
./User_Src/AFE_adbms6815_SM.d \
./User_Src/EEP_m24256a.d \
./User_Src/EEP_m24m02a.d \
./User_Src/FIRE_phsc220xcr.d \
./User_Src/GAS_sgxbld1.d \
./User_Src/HALL_cab1500.d \
./User_Src/HALL_s124.d \
./User_Src/HUMI_sht3xdis.d \
./User_Src/IMD_gqimdu2.d \
./User_Src/RTC_mcp7940a.d \
./User_Src/ms_Util.d \
./User_Src/ms_adc.d \
./User_Src/ms_alarm.d \
./User_Src/ms_boot.d \
./User_Src/ms_btms.d \
./User_Src/ms_can.d \
./User_Src/ms_can_mon.d \
./User_Src/ms_det.d \
./User_Src/ms_eep.d \
./User_Src/ms_flash.d \
./User_Src/ms_gpio.d \
./User_Src/ms_i2c.d \
./User_Src/ms_main.d \
./User_Src/ms_mvb.d \
./User_Src/ms_process.d \
./User_Src/ms_relay.d \
./User_Src/ms_rs232_mon.d \
./User_Src/ms_soc.d \
./User_Src/ms_spi.d \
./User_Src/ms_temp.d \
./User_Src/ms_timer.d \
./User_Src/ms_uart.d 


# Each subdirectory must supply rules for building sources it contributes
User_Src/%.o User_Src/%.su: ../User_Src/%.c User_Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../User_Inc -I../User_Src -I../Core/Inc -I../Core/Src -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User_Src

clean-User_Src:
	-$(RM) ./User_Src/AFE_adbms6815.d ./User_Src/AFE_adbms6815.o ./User_Src/AFE_adbms6815.su ./User_Src/AFE_adbms6815_SM.d ./User_Src/AFE_adbms6815_SM.o ./User_Src/AFE_adbms6815_SM.su ./User_Src/EEP_m24256a.d ./User_Src/EEP_m24256a.o ./User_Src/EEP_m24256a.su ./User_Src/EEP_m24m02a.d ./User_Src/EEP_m24m02a.o ./User_Src/EEP_m24m02a.su ./User_Src/FIRE_phsc220xcr.d ./User_Src/FIRE_phsc220xcr.o ./User_Src/FIRE_phsc220xcr.su ./User_Src/GAS_sgxbld1.d ./User_Src/GAS_sgxbld1.o ./User_Src/GAS_sgxbld1.su ./User_Src/HALL_cab1500.d ./User_Src/HALL_cab1500.o ./User_Src/HALL_cab1500.su ./User_Src/HALL_s124.d ./User_Src/HALL_s124.o ./User_Src/HALL_s124.su ./User_Src/HUMI_sht3xdis.d ./User_Src/HUMI_sht3xdis.o ./User_Src/HUMI_sht3xdis.su ./User_Src/IMD_gqimdu2.d ./User_Src/IMD_gqimdu2.o ./User_Src/IMD_gqimdu2.su ./User_Src/RTC_mcp7940a.d ./User_Src/RTC_mcp7940a.o ./User_Src/RTC_mcp7940a.su ./User_Src/ms_Util.d ./User_Src/ms_Util.o ./User_Src/ms_Util.su ./User_Src/ms_adc.d ./User_Src/ms_adc.o ./User_Src/ms_adc.su ./User_Src/ms_alarm.d ./User_Src/ms_alarm.o ./User_Src/ms_alarm.su ./User_Src/ms_boot.d ./User_Src/ms_boot.o ./User_Src/ms_boot.su ./User_Src/ms_btms.d ./User_Src/ms_btms.o ./User_Src/ms_btms.su ./User_Src/ms_can.d ./User_Src/ms_can.o ./User_Src/ms_can.su ./User_Src/ms_can_mon.d ./User_Src/ms_can_mon.o ./User_Src/ms_can_mon.su ./User_Src/ms_det.d ./User_Src/ms_det.o ./User_Src/ms_det.su ./User_Src/ms_eep.d ./User_Src/ms_eep.o ./User_Src/ms_eep.su ./User_Src/ms_flash.d ./User_Src/ms_flash.o ./User_Src/ms_flash.su ./User_Src/ms_gpio.d ./User_Src/ms_gpio.o ./User_Src/ms_gpio.su ./User_Src/ms_i2c.d ./User_Src/ms_i2c.o ./User_Src/ms_i2c.su ./User_Src/ms_main.d ./User_Src/ms_main.o ./User_Src/ms_main.su ./User_Src/ms_mvb.d ./User_Src/ms_mvb.o ./User_Src/ms_mvb.su ./User_Src/ms_process.d ./User_Src/ms_process.o ./User_Src/ms_process.su ./User_Src/ms_relay.d ./User_Src/ms_relay.o ./User_Src/ms_relay.su ./User_Src/ms_rs232_mon.d ./User_Src/ms_rs232_mon.o ./User_Src/ms_rs232_mon.su ./User_Src/ms_soc.d ./User_Src/ms_soc.o ./User_Src/ms_soc.su ./User_Src/ms_spi.d ./User_Src/ms_spi.o ./User_Src/ms_spi.su ./User_Src/ms_temp.d ./User_Src/ms_temp.o ./User_Src/ms_temp.su ./User_Src/ms_timer.d ./User_Src/ms_timer.o ./User_Src/ms_timer.su ./User_Src/ms_uart.d ./User_Src/ms_uart.o ./User_Src/ms_uart.su

.PHONY: clean-User_Src

