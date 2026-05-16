cmake_minimum_required(VERSION 3.20)
project(PlatformKit LANGUAGES CXX)

# =========================
# Source files for library
# =========================
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PlatformKit/*.cpp
)

option(PLATFORMKIT_BUILD_SHARED "Build PlatformKit as a shared library" OFF)

# =========================
# Library target
# =========================
if(PLATFORMKIT_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
else()
    add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
endif()

add_library(PlatformKit::PlatformKit ALIAS ${PROJECT_NAME})

# C++20 and warnings
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Preprocessor definition
target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_KIT_EXPORTS)

# Include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ===============================
# System dependency
# ===============================

if(PROJECT_IS_TOP_LEVEL)
    find_package(Vulkan REQUIRED)
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

# ===============================
# Other libraries
# ===============================

if(PROJECT_IS_TOP_LEVEL)
    find_package(CommonApi REQUIRED CONFIG)
#    find_package(Containers REQUIRED CONFIG)
endif()
target_link_libraries(PlatformKit PUBLIC CommonApi::CommonApi)
#target_link_libraries(PlatformKit PUBLIC Containers::Containers)

# =========================
# Test executable
# =========================
file(GLOB_RECURSE TEST_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
)

add_executable(PlatformKitTests ${TEST_SRC_FILES})

# Link the library
target_link_libraries(PlatformKitTests PRIVATE ${PROJECT_NAME})

# Match C++ standard and warnings
target_compile_features(PlatformKitTests PUBLIC cxx_std_20)
target_compile_options(PlatformKitTests PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_include_directories(PlatformKitTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Enable CTest
enable_testing()
add_test(NAME PlatformKitTests COMMAND PlatformKitTests)

# =========================
# Install rules
# =========================
install(TARGETS ${PROJECT_NAME}
    EXPORT PlatformKitTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)

install(EXPORT PlatformKitTargets
    FILE PlatformKitTargets.cmake
    NAMESPACE PlatformKit::
    DESTINATION lib/cmake/PlatformKit
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/install/PlatformKitConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/PlatformKitConfig.cmake
    INSTALL_DESTINATION lib/cmake/PlatformKit
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/PlatformKitConfig.cmake
    DESTINATION lib/cmake/PlatformKit
)