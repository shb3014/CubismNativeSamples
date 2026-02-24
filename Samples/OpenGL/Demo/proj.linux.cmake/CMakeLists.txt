cmake_minimum_required(VERSION 3.16)

set(APP_NAME Demo)
set(SDK_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
set(CORE_PATH ${SDK_ROOT_PATH}/Core)
set(FRAMEWORK_PATH ${SDK_ROOT_PATH}/Framework)
set(THIRD_PARTY_PATH ${SDK_ROOT_PATH}/Samples/OpenGL/thirdParty)
set(STB_PATH ${THIRD_PARTY_PATH}/stb)
set(RES_PATH ${SDK_ROOT_PATH}/Samples/Resources)
set(SAMPLE_SHADER_PATH ${SDK_ROOT_PATH}/Samples/OpenGL/Shaders/StandardES3)
set(FRAMEWORK_SHADER_PATH ${FRAMEWORK_PATH}/src/Rendering/OpenGL/Shaders/StandardES3)
set(COMMON_SRC_PATH ${SDK_ROOT_PATH}/Samples/Common)

project(${APP_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/${APP_NAME})
set(CMAKE_CONFIGURATION_TYPES Debug Release
  CACHE STRING "Configurations" FORCE
)
set(CMAKE_SUPPRESS_REGENERATION ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Cubism Core (ARM64 static lib)
add_library(Live2DCubismCore STATIC IMPORTED)
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION ${CORE_PATH}/lib/linux/arm64/libLive2DCubismCore.a
    INTERFACE_INCLUDE_DIRECTORIES ${CORE_PATH}/include
)

# Find EGL/GLES and DRM/GBM
find_package(PkgConfig REQUIRED)
pkg_check_modules(EGL REQUIRED egl glesv2)
pkg_check_modules(DRM REQUIRED libdrm gbm)

message(STATUS "Found EGL: ${EGL_LIBRARIES}")
message(STATUS "Found DRM/GBM: ${DRM_LIBRARIES}")

# Cubism Framework
set(FRAMEWORK_SOURCE OpenGL)
add_subdirectory(${FRAMEWORK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Framework)
target_compile_definitions(Framework PUBLIC CSM_TARGET_HARMONYOS_ES3)
target_include_directories(Framework PUBLIC ${EGL_INCLUDE_DIRS})
target_link_libraries(Framework
    Live2DCubismCore
    ${EGL_LIBRARIES}
    dl
)

# Demo executable
add_executable(${APP_NAME})
add_subdirectory(${COMMON_SRC_PATH} ${CMAKE_CURRENT_BINARY_DIR}/src/Common)
add_subdirectory(src)

target_link_libraries(${APP_NAME}
    PRIVATE
        Framework
        ${EGL_LIBRARIES}
        ${DRM_LIBRARIES}
        pthread
        dl
        m
)

target_include_directories(${APP_NAME} PRIVATE
    ${STB_PATH}
    ${EGL_INCLUDE_DIRS}
    ${DRM_INCLUDE_DIRS}
)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    target_link_options(${APP_NAME} PRIVATE "-Wl,--no-as-needed")
endif()

# Copy resources
add_custom_command(
    TARGET ${APP_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RES_PATH} $<TARGET_FILE_DIR:${APP_NAME}>/Resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SAMPLE_SHADER_PATH} $<TARGET_FILE_DIR:${APP_NAME}>/SampleShaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${FRAMEWORK_SHADER_PATH} $<TARGET_FILE_DIR:${APP_NAME}>/FrameworkShaders
)
