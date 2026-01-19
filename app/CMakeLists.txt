add_executable(${APP_NAME} main.cpp)

target_link_libraries(${APP_NAME}
        PRIVATE ${LIB_NAME}
)
