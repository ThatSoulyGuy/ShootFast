include_guard(GLOBAL)

include(FetchContent)

function(init_dependencies)
    FetchContent_Declare(
            glfw
            GIT_REPOSITORY "https://github.com/glfw/glfw.git"
            GIT_TAG master
    )

    FetchContent_MakeAvailable(glfw)

    if (NOT TARGET GLFW::GLFW)
        add_library(GLFW::GLFW ALIAS glfw)
    endif()

    set(ENET_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(ENET_FEATURE_IPV6 OFF CACHE BOOL "" FORCE)
    set(ENET_ENABLE_IPV6 OFF CACHE BOOL "" FORCE)
    set(ENET_IPV6 OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            enet
            GIT_REPOSITORY https://github.com/lsalzman/enet.git
            GIT_TAG master
    )

    FetchContent_MakeAvailable(enet)

    set(_enet_real "")

    foreach(cand enet ENet enetstatic enet_static)
        if(TARGET ${cand})
            set(_enet_real ${cand})
            break()
        endif()
    endforeach()

    if(_enet_real STREQUAL "")
        message(FATAL_ERROR "Could not find real ENet target; known candidates not present.")
    endif()

    if(MSVC)
        target_compile_options(${_enet_real} PRIVATE /UENET_FEATURE_IPV6 /UENET_ENABLE_IPV6 /UENET_IPV6)
    else()
        target_compile_options(${_enet_real} PRIVATE -UENET_FEATURE_IPV6 -UENET_ENABLE_IPV6 -UENET_IPV6)
    endif()

    target_compile_definitions(${_enet_real} PRIVATE ENET_FEATURE_IPV6=0 ENET_ENABLE_IPV6=0 ENET_IPV6=0)

    if(NOT TARGET enet::enet_static)
        add_library(enet::enet_static ALIAS ${_enet_real})
    endif()

    FetchContent_Declare(
            glad2
            GIT_REPOSITORY https://github.com/Dav1dde/glad.git
            GIT_TAG v2.0.6
    )

    FetchContent_MakeAvailable(glad2)

    include(${glad2_SOURCE_DIR}/cmake/GladConfig.cmake)

    glad_add_library(glad_global STATIC
            API "gl:core=4.1"
            LANGUAGE c
            LOCATION "${CMAKE_BINARY_DIR}/gladsources/glad_global"
    )

    if (NOT TARGET glad::glad)
        add_library(glad::glad ALIAS glad_global)
    endif()

endfunction()