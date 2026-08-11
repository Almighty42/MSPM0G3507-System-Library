cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/toolchain-arm-none-eabi.cmake)

project(mspm0g3507_app C ASM)

set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/mspm0g3507.ld)

add_executable(${PROJECT_NAME}
    src/mspm0g350x_startup.c
    src/mspm0g350x_system_init.c
    src/mspm0g350x_gpio.c
    src/mspm0g350x_iomux.c
    src/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra
    -ffunction-sections -fdata-sections
    -std=c11
    -g3
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Release>:-Os>
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${LINKER_SCRIPT}
    -nostartfiles
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--print-memory-usage
)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
)
