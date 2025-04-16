# How to use?

### 1. Create new project (empty)
### 2. Copy Drivers/CMSIS to project
### 3. Project -> Properties -> C/C++ General -> Paths and Symbols -> Source Location -> Add Folder "Drivers"
### 4. Project -> Properties -> C/C++ Build -> Settings -> MCU GCC (or G++) Compiler -> Include paths -> Add "../Drivers/CMSIS/Include" and "../Drivers/CMSIS/Device/ST/STM32F1xx/Include"
### 5. Project -> Properties -> C/C++ Build -> Settings -> MCU GCC (or G++) Compiler -> Preprocessor -> Add "(choose your MCU)" example "STM32F103xE"
### 6. In project add: #include "stm32f1xx.h"
