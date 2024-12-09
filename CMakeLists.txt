set(CMAKE_C_COMPILER "gcc")
project(dylibreloc)
set(CMAKE_C_FLAGS "-fexec-charset=UTF-8 -static -Os -DNDEBUG")

# 链接各种动态库, dylibreloc 将编写为以间址访问此动态库的静态库
#link_libraries(fastcpy)

# 设置输出目录
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 构建静态库, 需为每种库构建一个重定位库, 这里以 fastcpy 为例
# 静态库的符号重定位
add_library(implibfastcpy STATIC dylibreloc.h dylibreloc.c)
target_link_libraries(implibfastcpy PRIVATE lib/libtfastcpy.lib)
target_compile_definitions(implibfastcpy PRIVATE FASTCPY_STATIC_LIBRARY)
target_compile_definitions(implibfastcpy PRIVATE DYLIB_RELOC_USE_MUTEX)
set_target_properties(implibfastcpy PROPERTIES OUTPUT_NAME tfastcpy)
set_target_properties(implibfastcpy PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set_target_properties(implibfastcpy PROPERTIES COMPILE_FLAGS "-save-temps")

# 动态库的符号重定位
add_library(impdllfastcpy STATIC dylibreloc.h dylibreloc.c)
target_link_libraries(impdllfastcpy PRIVATE dylib/libtfastcpy.lib)
target_compile_definitions(impdllfastcpy PRIVATE DYLIB_RELOC_USE_MUTEX)
set_target_properties(impdllfastcpy PROPERTIES OUTPUT_NAME tfastcpy)
set_target_properties(impdllfastcpy PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dylib)
set_target_properties(impdllfastcpy PROPERTIES COMPILE_FLAGS "-save-temps")

# 可以构建使用 dylibreloc 及其动态库的应用程序
#add_executable(your_app dylibreloc.h your_source.c)
