cmake_minimum_required(VERSION 3.25)
project(OICQv2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")  # 指定pthread路径
include_directories(${PROJECT_SOURCE_DIR}/lib)

# 添加cJSON子目录, 设置cJSON库输出路径, 函数用于链接cjson库到指定的目标
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/cJSON)

set_target_properties(cjson PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/build"
        ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/build"
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/build"
        )

function(link_cjson_library TARGET)
    add_dependencies(${TARGET} cjson)
    target_link_libraries(${TARGET} cjson)
    set_target_properties(${TARGET} PROPERTIES
            BUILD_WITH_INSTALL_RPATH TRUE
            INSTALL_RPATH "${PROJECT_BINARY_DIR}/build"
            )
endfunction()

# 创建可执行文件
add_executable(server src/server.c lib/shared_memory_dir/shared_memory.c lib/shared_memory_dir/shared_memory.h)
add_executable(client src/client.c lib/file_db_operate/operate_user_info.h lib/basic_info/basic_definition.h lib/file_db_operate/operate_chat_history.h lib/socket_tools/basic_struct_transfer.h lib/common_tools/common_tools.h lib/socket_tools/struct_json_transfer.h lib/socket_server/auto_server_transfer.h lib/socket_client/auto_client_transfer.h lib/socket_server/server_socket_handler.h lib/socket_client/client_socket_sender.h lib/socket_server/server_socket_sender.h lib/socket_server/server_socket_single_broadcast.h lib/shared_memory_dir/shared_memory.c lib/shared_memory_dir/shared_memory.h)
add_executable(test src/test.c)

# 链接cjson库到各个可执行文件
link_cjson_library(server)
link_cjson_library(client)
link_cjson_library(test)

# 自动查找openssl
#find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})
#target_link_libraries(server ${OPENSSL_LIBRARIES})
#target_link_libraries(client ${OPENSSL_LIBRARIES})
#target_link_libraries(test ${OPENSSL_LIBRARIES})
