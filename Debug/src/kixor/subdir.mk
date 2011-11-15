################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/kixor/list.cpp \
../src/kixor/objLoader.cpp \
../src/kixor/obj_parser.cpp \
../src/kixor/string_extra.cpp 

OBJS += \
./src/kixor/list.o \
./src/kixor/objLoader.o \
./src/kixor/obj_parser.o \
./src/kixor/string_extra.o 

CPP_DEPS += \
./src/kixor/list.d \
./src/kixor/objLoader.d \
./src/kixor/obj_parser.d \
./src/kixor/string_extra.d 


# Each subdirectory must supply rules for building sources it contributes
src/kixor/%.o: ../src/kixor/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


