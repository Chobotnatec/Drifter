################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../FreeIMUTest.cpp \
../HBridgeMotor.cpp \
../PacketUtils.cpp 

OBJS += \
./FreeIMUTest.o \
./HBridgeMotor.o \
./PacketUtils.o 

CPP_DEPS += \
./FreeIMUTest.d \
./HBridgeMotor.d \
./PacketUtils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\arduino-1.0.2-windows\arduino-1.0.2\hardware\arduino\cores\arduino" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\hardware\arduino\variants\mega" -I"C:\Users\Martin Skalsk�\Desktop\Dropbox\C++\Arduino\FreeIMUTest.h" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\Wire" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\MPU60X0" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\I2Cdev" -I"C:\arduino-1.0.2-windows\arduino-1.0.2\libraries\SPI" -D__IN_ECLIPSE__=1 -DUSB_VID= -DUSB_PID= -DARDUINO=102 -Wall -Os -ffunction-sections -fdata-sections -fno-exceptions -g -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -c -o "$@" -x c++ "$<"
	@echo 'Finished building: $<'
	@echo ' '


