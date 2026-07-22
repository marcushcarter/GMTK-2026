add_library(app_compiler_options INTERFACE)

target_compile_options(app_compiler_options INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /MP>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)