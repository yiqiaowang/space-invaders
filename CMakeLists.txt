cmake_minimum_required(VERSION 3.13)

#=============== vcpkg Bootstrap ==============# 

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

message(" [INFO] VCPKG CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")

#============== clangd Bootstrap ==============# 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#======= Global Project Configuration =========#

set(PROJECT_NAME
        DummyProject # Change this!
)

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)     

#========== Library Configurations ============#
find_package(spdlog CONFIG REQUIRED)
find_package(doctest CONFIG REQUIRED)



#========== Targets Configurations ============#

## ==> Target: main - Executable: main
add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE
        spdlog::spdlog
)

## ==> Target: test - Executable: test
add_executable(test test/test.cpp)
target_link_libraries(test PRIVATE
        doctest::doctest
)
