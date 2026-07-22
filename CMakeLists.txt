cmake_minimum_required(VERSION 3.28)
project(Ballistic VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /I\"${CMAKE_SOURCE_DIR}\" /I\"${CMAKE_BINARY_DIR}/generated\"")

include(CompilerOptions)
include(Platform)
include(EmbedAssets)

add_subdirectory(thirdparty)
add_subdirectory(engine)

set(BALLISTIC_GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${BALLISTIC_GENERATED_DIR})

app_generate_asset_rc(
    ${BALLISTIC_GENERATED_DIR}/embedded_assets.rc
    ${CMAKE_SOURCE_DIR}/assets/textures
    ${CMAKE_SOURCE_DIR}/assets/shaders
)

set(BALLISTIC_ASSETS ${BALLISTIC_GENERATED_DIR}/embedded_assets.rc)

add_subdirectory(platform/windows)