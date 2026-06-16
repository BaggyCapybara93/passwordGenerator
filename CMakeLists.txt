cmake_minimum_required(VERSION 3.10)
project(PasswordGenerator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wsign-conversion -Wcast-qual")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast -Wshadow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2 -Wformat-security")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")

# Build type defaults
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

set(SOURCE_FILES
    src/random.cpp
    src/ui.cpp
    src/parse_arguments.cpp
    src/main.cpp
    src/password/password_generator.cpp
    src/password/password_entropy.cpp
    src/password/password_blacklist.cpp
    src/password/password_honey.cpp
)

add_executable(PasswordGenerator ${SOURCE_FILES})

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

find_package(Boost 1.71 REQUIRED COMPONENTS program_options)

target_include_directories(PasswordGenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(PasswordGenerator PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(PasswordGenerator PRIVATE Boost::program_options)
