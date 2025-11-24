cmake_minimum_required(VERSION 3.11)
project(kidsvm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(kidsvm ${SRC})
target_compile_options(kidsvm PRIVATE -Wall -Wextra -Og -g)
