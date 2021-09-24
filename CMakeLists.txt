cmake_minimum_required(VERSION 3.19)
project(HxA)
add_executable(HxaTool
    f_text.c
    forge.h
    hxa_fbx_convert.c
    hxa_load_save.c
    hxa_print.c
    hxa_upi_command_line_interface.c
    hxa_util_close.c
    hxa_util_convert.c
    hxa_util_fbx_parse.c
    hxa_util_font_process.c
    hxa_util_inflate.c
    hxa_util_merge_split.c
    hxa_util_neighbour.c
    hxa_util_triangulate.c
    hxa_util_true_type_parse.c
    hxa_util_validate.c
    hxa_util_vertex_purge.c
)
set(HXA_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
#target_link_libraries(HxaTool ...)
#target_include_directories(HxaTool ${HXA_ROOT})
install(TARGETS HxaTool RUNTIME DESTINATION bin)

