################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/arduino-1.0.2-windows/arduino-1.0.2/libraries/Wire/Wire.cpp 

OBJS += \
./Wire/Wire.o 

CPP_DEPS += \
./Wire/Wire.d 


# Each subdirectory must supply rules for building sources it contributes
Wire/Wire.o: C:/arduino-1.0.2-windows/arduino-1.0.2/libraries/Wire/Wire.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\arduino-1.0.2-windows\arduino-1.0.2\hardware\arduino\cores\arduino" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\hardware\arduino\variants\mega" -I"C:\Users\Martin Skalsk�\Desktop\Dropbox\C++\Arduino\FreeIMUTest.h" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\Wire" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\Wire\utility" -D__IN_ECLIPSE__=1 -DUSB_VID= -DUSB_PID= -DARDUINO=102 -Wall -Os -ffunction-sections -fdata-sections -fno-exceptions -g -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


