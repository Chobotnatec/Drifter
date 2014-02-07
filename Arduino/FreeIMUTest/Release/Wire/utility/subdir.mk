################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/arduino-1.0.2-windows/arduino-1.0.2/libraries/Wire/utility/twi.c 

OBJS += \
./Wire/utility/twi.o 

C_DEPS += \
./Wire/utility/twi.d 


# Each subdirectory must supply rules for building sources it contributes
Wire/utility/twi.o: C:/arduino-1.0.2-windows/arduino-1.0.2/libraries/Wire/utility/twi.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"C:\arduino-1.0.2-windows\arduino-1.0.2\hardware\arduino\cores\arduino" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\hardware\arduino\variants\mega" -I"C:\Users\Martin Skalský\Desktop\Dropbox\C++\Arduino\FreeIMUTest.h" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\Wire" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\Wire\utility" -D__IN_ECLIPSE__=1 -DARDUINO=102 -DUSB_PID= -DUSB_VID= -Wall -Os -g -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


