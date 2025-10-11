include_guard(GLOBAL)

include(FetchContent)

function(shootfast_glad_add_library TARGET)
    set(options SHARED STATIC MODULE INTERFACE EXCLUDE_FROM_ALL REPRODUCIBLE QUIET)
    set(oneValueArgs LOCATION LANGUAGE)
    set(multiValueArgs API EXTENSIONS)
    cmake_parse_arguments(GG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT GG_LOCATION)
        set(GG_LOCATION "${CMAKE_BINARY_DIR}/gladsources/${TARGET}")
    endif()
    if(NOT GG_API)
        message(FATAL_ERROR "shootfast_glad_add_library: Need API (e.g. API \"gl=4.1\")")
    endif()

    file(MAKE_DIRECTORY "${GG_LOCATION}")
    find_package(Python COMPONENTS Interpreter REQUIRED)

    set(_args
            --out-path "${GG_LOCATION}"
            --api "${GG_API}"
            --generator c
            --profile core
            --reproducible
            --local-files
    )

    if(GG_QUIET)
        list(APPEND _args --quiet)
    endif()

    if(GG_EXTENSIONS)
        list(JOIN GG_EXTENSIONS "," _exts)
        list(APPEND _args --extensions "${_exts}")
    endif()

    set(_khr "${GG_LOCATION}/khrplatform.h")
    set(_gl_h "${GG_LOCATION}/glad.h")
    set(_gl_c "${GG_LOCATION}/glad.c")

    add_custom_command(
            OUTPUT "${_khr}" "${_gl_h}" "${_gl_c}"
            COMMAND ${CMAKE_COMMAND} -E env GLAD_SOURCES_DIR=${glad2_SOURCE_DIR}
            "${Python_EXECUTABLE}" -m glad ${_args}
            COMMENT "Generating GLAD sources for ${TARGET} (offline, flat)"
            VERBATIM
    )

    if(GG_INTERFACE)
        add_library("${TARGET}" INTERFACE)
        target_include_directories("${TARGET}" INTERFACE "${GG_LOCATION}")
    else()
        set(_libtype STATIC)
        if(GG_SHARED)
            set(_libtype SHARED)
        endif()

        if(GG_MODULE)
            set(_libtype MODULE)
        endif()
        add_library("${TARGET}" ${_libtype} "${_khr}" "${_gl_h}" "${_gl_c}")
        target_include_directories("${TARGET}" PUBLIC "${GG_LOCATION}")
        target_link_libraries("${TARGET}" PUBLIC ${CMAKE_DL_LIBS})
    endif()
endfunction()

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
            GIT_TAG v2.0.8
    )

    FetchContent_MakeAvailable(glad2)

    include(${glad2_SOURCE_DIR}/cmake/GladConfig.cmake)

    shootfast_glad_add_library(glad_global STATIC
            API "gl=4.1"
            LOCATION "${CMAKE_BINARY_DIR}/gladsources/glad_global"
    )

    if (NOT TARGET glad::glad)
        add_library(glad::glad ALIAS glad_global)
    endif()

    set(FREEIMAGE_BUILD_WRAPPER OFF CACHE BOOL "" FORCE)

    set(FREEIMAGE_BUILD_STATIC ON  CACHE BOOL "" FORCE)
    set(FREEIMAGE_BUILD_SHARED OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            FreeImage
            GIT_REPOSITORY https://github.com/danoli3/FreeImage.git
            GIT_TAG master
    )

    FetchContent_MakeAvailable(FreeImage)

    if (TARGET FreeImage)
        target_compile_definitions(FreeImage PUBLIC FREEIMAGE_LIB)
    endif()

    if(NOT TARGET FreeImage::FreeImage)
        add_library(FreeImage::FreeImage ALIAS FreeImage)
    endif()

    if(APPLE AND TARGET FreeImage)
        target_compile_options(FreeImage PRIVATE
                -U__MWERKS__
                -Umacintosh
                -Uapplec
                -UTHINK_C
                -U__SC__
                -UTARGET_OS_MAC
        )

        target_compile_definitions(FreeImage PRIVATE
                PNG_NO_FLOATING_POINT_SUPPORTED
                PNG_NO_FLOATING_ARITHMETIC_SUPPORTED
        )
    endif()
endfunction()