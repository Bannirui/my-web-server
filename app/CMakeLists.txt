# 拷贝资源文件到构建目录和运行目录
set(ASSET_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/www")
set(ASSET_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/www")
set(ASSET_RUN_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/www")

if (EXISTS "${ASSET_SRC_DIR}")
    add_custom_target(copy_assets
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSET_SRC_DIR}" "${ASSET_BIN_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSET_SRC_DIR}" "${ASSET_RUN_DIR}"
            COMMENT "Copying assets to build and runtime directories"
    )
endif ()

function(add_app name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE ${LIB_NAME})
    # 构建时触发资源文件复制
    if (TARGET copy_assets)
        add_dependencies(${name} copy_assets)
    endif ()
endfunction()

add_app(my-web-server server_main.cpp)
add_app(my-web-client client_main.cpp)
