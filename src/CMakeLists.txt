# 源文件
file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HDRS "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/include/*.hpp")

# 添加头文件的目的仅仅是为了让IDEA识别到 新建文件的时候添加索引
add_library(${LIB_NAME} ${SRCS} ${HDRS})

target_include_directories(${LIB_NAME}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${LIB_NAME} PUBLIC spdlog)
