function(add_app name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE ${LIB_NAME})
endfunction()

add_app(my-web-server server_main.cpp)
add_app(my-web-client client_main.cpp)
