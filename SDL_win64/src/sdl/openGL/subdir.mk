################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sdl/openGL/cube3d.c \
../src/sdl/openGL/no_effect.c 

OBJS += \
./src/sdl/openGL/cube3d.o \
./src/sdl/openGL/no_effect.o 

C_DEPS += \
./src/sdl/openGL/cube3d.d \
./src/sdl/openGL/no_effect.d 


# Each subdirectory must supply rules for building sources it contributes
src/sdl/openGL/%.o: ../src/sdl/openGL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	x86_64-w64-mingw32-gcc -DMINGW64 -DSDL -DGLEW_STATIC -I../src -I../src/sdl -I/usr/x86_64-w64-mingw32/usr/include/SDL -I/home/fhorse/sviluppo/personale/punes/misc/DXSDK/Include -I/home/fhorse/sviluppo/personale/punes/misc/DXSDK/vc10/include -O3 -Wall -ffast-math -c -fmessage-length=0 -finline-functions -Winline -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


