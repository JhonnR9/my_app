cmake_minimum_required(VERSION 3.31)
project(my_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WIN32_EXECUTABLE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DUNICODE -D_UNICODE)

add_executable(my_app
        src/main.cpp
        src/app.cpp
        src/scenes/my_scene.cpp
        src/scenes/my_scene.h
        src/systems/system.h
)

add_subdirectory(external/DirectXTK)
# EnTT
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external/entt/single_include")
# nlohmann
add_library(nlohmann INTERFACE)
target_include_directories(nlohmann INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/external/nlohmann/include")


target_link_libraries(my_app PRIVATE
        d3d11.lib
        dxgi.lib
        d3dcompiler.lib
        user32.lib
        gdi32.lib
        kernel32.lib
        DirectXTK
        entt
        nlohmann
)

set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
set(RESOURCE_DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources")

add_custom_command(TARGET my_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RESOURCE_DIR}" "${RESOURCE_DEST_DIR}"
)