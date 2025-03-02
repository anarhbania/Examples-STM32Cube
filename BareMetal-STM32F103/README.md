# How install bare metal drivers to Project:
- create new Project, set Empty Project
- download https://github.com/STMicroelectronics/cmsis-device-f1
- copy "CMSIS Device" to "Device" folder
- set Project -> Properties -> C/C++ General -> Paths and Symbols -> Source Location -> Add Folder "Drivers"
- set Project -> Properties -> C/C++ Build -> Settings -> MCU GCC Compiler -> Include paths -> Add "../Drivers/CMSIS/Include"
- set Project -> Properties -> C/C++ Build -> Settings -> MCU GCC Compiler -> Include paths -> Add "../Drivers/CMSIS/Device/ST/STM32F1xx/Include"
- set Project -> Properties -> C/C++ Build -> Settings -> MCU GCC Compiler -> Preprocessor -> Add "(set your MCU code, for example STM32F103xE)"
- in main.c add #include "stm32f1xx.h"