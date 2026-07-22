


add_executable(gmtk_app 
app_res.rc
${BALLISTIC_ASSETS}
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(gmtk_app PRIVATE app_core app_compiler_options)
target_compile_definitions(gmtk_app PRIVATE)
target_include_directories(gmtk_app  PRIVATE ${CMAKE_SOURCE_DIR})
target_link_options(gmtk_app PRIVATE /MANIFEST:NO)