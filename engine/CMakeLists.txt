file(GLOB_RECURSE APP_CORE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(app_core STATIC ${APP_CORE_SOURCES})

target_include_directories(app_core
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/generated
)

target_link_libraries(app_core
PUBLIC
    app_compiler_options
    glad
    glfw
    glm
    stb
    imgui
)

target_compile_definitions(app_core PRIVATE DEV_TOOLS)