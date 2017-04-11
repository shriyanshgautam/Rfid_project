################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../curl-7.40.0-devel-mingw64/samples/chkspeed.c \
../curl-7.40.0-devel-mingw64/samples/simple.c 

OBJS += \
./curl-7.40.0-devel-mingw64/samples/chkspeed.o \
./curl-7.40.0-devel-mingw64/samples/simple.o 

C_DEPS += \
./curl-7.40.0-devel-mingw64/samples/chkspeed.d \
./curl-7.40.0-devel-mingw64/samples/simple.d 


# Each subdirectory must supply rules for building sources it contributes
curl-7.40.0-devel-mingw64/samples/%.o: ../curl-7.40.0-devel-mingw64/samples/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


