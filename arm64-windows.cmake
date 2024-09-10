# aarch64-windows.cmake
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(target arm64-windows)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_COMPILER_TARGET ${target})
set(CMAKE_CXX_COMPILER_TARGET ${target})

set(CMAKE_C_FLAGS "-target aarch64-windows")
set(CMAKE_CXX_FLAGS "-target aarch64-windows")