project(web-server VERSION 1.0 LANGUAGES CXX)

# 依赖的三方库
include(${CMAKE_SOURCE_DIR}/cmake/spdlog.cmake)

# 开始编译自己的项目
file(GLOB_RECURSE srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE hdrs "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

# 生成库文件 添加头文件的目的仅仅是为了让IDEA识别到 新建文件的时候添加索引
add_library(${PROJECT_NAME} STATIC ${srcs} ${hdrs})

target_include_directories(${PROJECT_NAME}
        SYSTEM PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
# lib
target_link_libraries(${PROJECT_NAME}
        PUBLIC spdlog
)
