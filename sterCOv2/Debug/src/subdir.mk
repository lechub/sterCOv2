################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/_write.c \
../src/eeprom.c \
../src/stm32f4xx_flash.c \
../src/system_stm32f4xx.c 

CPP_SRCS += \
../src/EditValue.cpp \
../src/HMI.cpp \
../src/Hardware.cpp \
../src/Menu.cpp \
../src/Parameter.cpp \
../src/Pomiar.cpp \
../src/Praca.cpp \
../src/QuickTask.cpp \
../src/TextLcd.cpp \
../src/main.cpp \
../src/st7032i.cpp 

OBJS += \
./src/EditValue.o \
./src/HMI.o \
./src/Hardware.o \
./src/Menu.o \
./src/Parameter.o \
./src/Pomiar.o \
./src/Praca.o \
./src/QuickTask.o \
./src/TextLcd.o \
./src/_write.o \
./src/eeprom.o \
./src/main.o \
./src/st7032i.o \
./src/stm32f4xx_flash.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/_write.d \
./src/eeprom.d \
./src/stm32f4xx_flash.d \
./src/system_stm32f4xx.d 

CPP_DEPS += \
./src/EditValue.d \
./src/HMI.d \
./src/Hardware.d \
./src/Menu.d \
./src/Parameter.d \
./src/Pomiar.d \
./src/Praca.d \
./src/QuickTask.d \
./src/TextLcd.d \
./src/main.d \
./src/st7032i.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F411xE -DUSE_HAL_DRIVER_NO -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I../lbutils -I../src -I../myDrivers -std=gnu++14 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -Wabi -Wctor-dtor-privacy -Wnoexcept -Wnon-virtual-dtor -Wstrict-null-sentinel -Wsign-promo -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F411xE -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I../lbutils -I../src -I../myDrivers -std=gnu11 -Wmissing-prototypes -Wstrict-prototypes -Wbad-function-cast -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


