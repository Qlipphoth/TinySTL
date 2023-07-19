# 将 ${PROJECT_SOURCE_DIR}/TinySTL 目录添加到包含文件的搜索路径中。
include_directories(${PROJECT_SOURCE_DIR}/TinySTL)
# 设置一个变量 APP_SRC，其值为 test.cpp。这个变量表示可执行文件的源代码文件。
set(APP_SRC test.cpp)
# 置一个变量 EXECUTABLE_OUTPUT_PATH，其值为 ${PROJECT_SOURCE_DIR}/bin。
# 这个变量表示生成的可执行文件的输出路径，即将可执行文件放置在 
# ${PROJECT_SOURCE_DIR}/bin 目录下。
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 生成可执行文件 stltest。
add_executable(stltest ${APP_SRC})