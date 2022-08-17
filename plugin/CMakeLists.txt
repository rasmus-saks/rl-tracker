cmake_minimum_required(VERSION 3.15.0)

project(StreamPlugin)

set(BAKKESMOD_ROOT ${PROJECT_SOURCE_DIR}/third-party/bakkesmodsdk)
set(ASIO_ROOT ${PROJECT_SOURCE_DIR}/third-party/asio)
set(JSON_ROOT ${PROJECT_SOURCE_DIR}/third-party/json)
set(SPDLOG_ROOT ${PROJECT_SOURCE_DIR}/third-party/spdlog)


set(SOURCES
    src/main/cpp/StreamPlugin.cpp
    src/main/cpp/tcp_connection.cpp
    src/main/cpp/tcp_server.cpp
)

set(TESTAPP_SOURCES
    src/main/cpp/Test.cpp
    src/main/cpp/tcp_connection.cpp
    src/main/cpp/tcp_server.cpp
)

set(PUBLIC_INCLUDES
    ${BAKKESMOD_ROOT}/include
    ${SPDLOG_ROOT}/include
    ${ASIO_ROOT}/include
    ${JSON_ROOT}/include
)

set(PRIVATE_INCLUDES
    ${PROJECT_SOURCE_DIR}/src/main/headers
    ${PROJECT_BINARY_DIR}
)
set(LIBRARIES
    ${BAKKESMOD_ROOT}/lib/pluginsdk.lib
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/release)
set(_WIN32_WINNT 0x0601)


add_library(StreamPlugin SHARED ${SOURCES})
add_executable(TestApp WIN32 ${TESTAPP_SOURCES})

target_compile_options(StreamPlugin PRIVATE /Zi)

# Tell linker to include symbol data
set_target_properties(StreamPlugin PROPERTIES
    LINK_FLAGS "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"
)

# Set file name & location
set_target_properties(StreamPlugin PROPERTIES
    COMPILE_PDB_NAME StreamPlugin
    COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR}
)

target_include_directories(TestApp PUBLIC ${PUBLIC_INCLUDES} PRIVATE ${PRIVATE_INCLUDES})
target_link_libraries(TestApp PUBLIC ${LIBRARIES})
target_include_directories(StreamPlugin PUBLIC ${PUBLIC_INCLUDES} PRIVATE ${PRIVATE_INCLUDES})
target_link_libraries(StreamPlugin PUBLIC ${LIBRARIES} INTERFACE "-Wl,--allow-multiple-definition")

set_target_properties(TestApp PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

