cmake_minimum_required(VERSION 3.31)
project(my_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WIN32_EXECUTABLE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DUNICODE -D_UNICODE)

include_directories("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um")
include_directories("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/shared")
include_directories("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/winrt")
include_directories("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/ucrt")
include_directories("C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0/um")


add_executable(my_app
        src/main.cpp
        src/app.cpp
)

target_link_libraries(my_app
        d3d11.lib
        dxgi.lib
        d3dcompiler.lib
        user32.lib
        gdi32.lib
        kernel32.lib
)
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
set(RESOURCE_DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources")


add_custom_command(TARGET my_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RESOURCE_DIR}" "${RESOURCE_DEST_DIR}"
)
