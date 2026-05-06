cmake_minimum_required(VERSION 3.10)

project(PasswordGenerator VERSION 1.0 LANGUAGES CXX)

# Set C++ Standard 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "C++ flags for release build.")
endif()

set(SOURCES 
    src/random.cpp
    src/ui.cpp
    src/main.cpp
)

set(HEADERS 
    include/random.hpp
    include/ui.hpp
)

add_library(rng_lib STATIC ${SOURCES})
target_include_directories(rng_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} 
    ${SOURCES}
)

# Link the RNG Library 
target_link_libraries(${PROJECT_NAME} PRIVATE rng_lib)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Install Rules
install(TARGETS ${PROJECT_NAME} 
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)

message(STATUS "CMake Configuration Complete.")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
