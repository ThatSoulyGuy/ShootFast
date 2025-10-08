cmake_minimum_required(VERSION 4.0)
project(ShootFast)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(Dependencies.cmake)

if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:preprocessor /Zc:__cplusplus)
endif()

file(GLOB_RECURSE CLIENT_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ShootFast/Header/Client/*.hpp" "${CMAKE_SOURCE_DIR}/ShootFast/Header/Independent/*.hpp")
file(GLOB_RECURSE CLIENT_SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ShootFast/Source/Client/*.cpp" "${CMAKE_SOURCE_DIR}/ShootFast/Source/Independent/*.cpp")

file(GLOB_RECURSE SERVER_HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ShootFast/Header/Server/*.hpp" "${CMAKE_SOURCE_DIR}/ShootFast/Header/Independent/*.hpp")
file(GLOB_RECURSE SERVER_SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/ShootFast/Source/Server/*.cpp" "${CMAKE_SOURCE_DIR}/ShootFast/Source/Independent/*.cpp")

message(STATUS "Client sources:\n${CLIENT_SOURCE_FILES}")

init_dependencies()

function(shoot_fast_add_executable target)
    add_executable(${target} ${ARGN})

    target_include_directories(${target} PRIVATE "${CMAKE_BINARY_DIR}/_deps/enet-src/include/")
    target_include_directories(${target} PRIVATE "${CMAKE_SOURCE_DIR}/Library/Header/")
    target_include_directories(${target} PRIVATE "${CMAKE_SOURCE_DIR}/ShootFast/Header/")

    if (MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${target} PRIVATE stdc++exp)
    endif()

    target_link_libraries(${target} PRIVATE enet::enet_static)
endfunction()

shoot_fast_add_executable(Client ${CLIENT_SOURCE_FILES} ${CLIENT_HEADER_FILES})
shoot_fast_add_executable(Server ${SERVER_SOURCE_FILES} ${SERVER_HEADER_FILES})

target_include_directories(Client PRIVATE "${FreeImage_SOURCE_DIR}/Source")

if(APPLE)
    target_link_options(Client PRIVATE "-Wl,-framework,Cocoa" "-Wl,-framework,IOKit" "-Wl,-framework,CoreVideo")
elseif(WIN32)
    target_link_libraries(Client PRIVATE opengl32)
endif()

target_link_libraries(Client PRIVATE glfw glad_global FreeImage::FreeImage)

target_compile_definitions(Client PUBLIC SHOOTFAST_HAVE_FREEIMAGE)
target_compile_definitions(Server PUBLIC IS_SERVER_FLAG)

set(SOURCE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
set(BUILD_ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/Assets")

add_custom_command(TARGET Client POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_ASSETS_DIR} ${BUILD_ASSETS_DIR} COMMENT "Copying assets to build folder...")