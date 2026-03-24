cmake_minimum_required(VERSION 3.16)
project(CHIP8-simulator VERSION 0.1 LANGUAGES CXX)

# 使用 C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 源码文件
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CHIP-8-simulator")
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/core.cpp
    ${SRC_DIR}/platform.cpp
)

# 可选构建类型默认
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# 尝试使用 SDL3 CMake package，若不可用则使用 pkg-config
find_package(SDL3 CONFIG)
if(SDL3_FOUND)
  set(SDL3_TARGET SDL3::SDL3)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL3 REQUIRED sdl3)
endif()

# 可执行目标
add_executable(chip8-simulator ${SOURCES})

target_include_directories(chip8-simulator PRIVATE ${SRC_DIR})

if(TARGET SDL3::SDL3)
  target_link_libraries(chip8-simulator PRIVATE SDL3::SDL3)
elseif(TARGET SDL3)
  target_link_libraries(chip8-simulator PRIVATE SDL3)
else()
  # 使用 pkg-config 导出的变量
  target_include_directories(chip8-simulator PRIVATE ${SDL3_INCLUDE_DIRS})
  target_link_libraries(chip8-simulator PRIVATE ${SDL3_LIBRARIES})
  target_compile_options(chip8-simulator PRIVATE ${SDL3_CFLAGS_OTHER})
endif()

# 安装规则（可选）
install(TARGETS chip8-simulator RUNTIME DESTINATION bin)

message(STATUS "Project: ${PROJECT_NAME} (C++ ${CMAKE_CXX_STANDARD})")
message(STATUS "Sources: ${SOURCES}")

# 可选：启用编译器警告
if(MSVC)
  target_compile_options(chip8-simulator PRIVATE /W4 /permissive-)
else()
  target_compile_options(chip8-simulator PRIVATE -Wall -Wextra -Wpedantic)
endif()
