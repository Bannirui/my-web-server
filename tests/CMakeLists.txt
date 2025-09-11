project(tests
        VERSION 1.0
        LANGUAGES C CXX
)
# 源文件
file(GLOB_RECURSE srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# 添加头文件的目的仅仅是为了让IDEA识别到 新建文件的时候添加索引
add_executable(${PROJECT_NAME} ${srcs})
target_link_libraries(${PROJECT_NAME} PUBLIC web-server)