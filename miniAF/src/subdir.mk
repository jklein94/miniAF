################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FileParser.c \
../Labels.c \
../Semantics.c \
../Vector.c \
../main.c \
../map.c \
../Util.c

OBJS += \
./FileParser.o \
./Labels.o \
./Semantics.o \
./Vector.o \
./main.o \
./map.o \
./Util.o

C_DEPS += \
./FileParser.d \
./Labels.d \
./Semantics.d \
./Vector.d \
./main.d \
./map.d \
./Util.d

DIR := ${CURDIR}
# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I DIR -O3 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


