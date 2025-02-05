cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME Euler-Verlet-RK4)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

# Define SFML directory
set(SFML_DIR "C:/Libraries/SFML/SFML-2.6.2/lib/cmake/SFML")

# Find SFML
find_package(SFML 2.6.2 REQUIRED COMPONENTS graphics window system)

# Create main executable
add_executable(main main.cpp)
target_link_libraries(main sfml-graphics sfml-window sfml-system)