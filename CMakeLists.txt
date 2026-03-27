cmake_minimum_required(VERSION 3.16)
cmake_minimum_required(VERSION 3.16)

project(CHIP8-simulator VERSION 0.1 LANGUAGES CXX)

# Use C++17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CHIP-8-simulator")
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/core.cpp
    ${SRC_DIR}/platform.cpp
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Prefer the SDL3 CMake package. Fall back to pkg-config if needed.
find_package(SDL3 CONFIG)
if(SDL3_FOUND)
  set(SDL3_TARGET SDL3::SDL3)
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL3 REQUIRED sdl3)
endif()

add_executable(chip8-simulator ${SOURCES})

target_include_directories(chip8-simulator PRIVATE ${SRC_DIR})

if(TARGET SDL3::SDL3)
  target_link_libraries(chip8-simulator PRIVATE SDL3::SDL3)
elseif(TARGET SDL3)
  target_link_libraries(chip8-simulator PRIVATE SDL3)
else()
  target_include_directories(chip8-simulator PRIVATE ${SDL3_INCLUDE_DIRS})
  target_link_libraries(chip8-simulator PRIVATE ${SDL3_LIBRARIES})
  target_compile_options(chip8-simulator PRIVATE ${SDL3_CFLAGS_OTHER})
endif()

install(TARGETS chip8-simulator RUNTIME DESTINATION bin)

message(STATUS "Project: ${PROJECT_NAME} (C++ ${CMAKE_CXX_STANDARD})")
message(STATUS "Sources: ${SOURCES}")

if(MSVC)
  target_compile_options(chip8-simulator PRIVATE /W4 /permissive-)
else()
  target_compile_options(chip8-simulator PRIVATE -Wall -Wextra -Wpedantic)
endif()
