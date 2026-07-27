cmake_minimum_required(VERSION 3.20)
project(mspm0g3507_syslib C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/startup_mspm0g3507.c
    src/main.c
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/mspm0g3507.ld
    -Wl,-Map=${PROJECT_NAME}.map
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}
)
