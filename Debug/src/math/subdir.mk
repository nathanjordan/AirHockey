################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CXX_SRCS += \
../src/math/mat2.cxx \
../src/math/mat3.cxx \
../src/math/mat4.cxx 

OBJS += \
./src/math/mat2.o \
./src/math/mat3.o \
./src/math/mat4.o 

CXX_DEPS += \
./src/math/mat2.d \
./src/math/mat3.d \
./src/math/mat4.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/%.o: ../src/math/%.cxx
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


