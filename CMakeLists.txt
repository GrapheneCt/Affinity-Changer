cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_C_COMPILER "arm-vita-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-vita-eabi-g++")

project(AffinityChanger)

set(CMAKE_C_FLAGS "-Wl,-q -O3 -std=c99")

add_executable(${PROJECT_NAME}.elf
	src/main.c
	src/ctrl.c
)

target_link_libraries(${PROJECT_NAME}.elf
	-lSceDisplay_stub
	-lSceCtrl_stub
	SceIofilemgr_stub 
	SceVshBridge_stub 
	ScePower_stub 
	SceShellSvc_stub 
)

add_custom_target(${PROJECT_NAME}.vpk ALL
	COMMAND vita-elf-create ${PROJECT_NAME}.elf ${PROJECT_NAME}.velf
	COMMAND vita-make-fself ${PROJECT_NAME}.velf eboot.bin
	COMMAND vita-mksfoex -s TITLE_ID=CPUAFFCTR -d ATTRIBUTE=32768 "${PROJECT_NAME}" param.sfo
	COMMAND vita-pack-vpk -s param.sfo -b eboot.bin ${PROJECT_NAME}.vpk
)
add_dependencies(${PROJECT_NAME}.vpk ${PROJECT_NAME}.elf)
