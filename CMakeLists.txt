cmake_minimum_required(VERSION 3.14)
project(raylib_module LANGUAGES C CXX)

# Set up cache directory for built artifacts
set(CACHE_DIR "${CMAKE_BINARY_DIR}/cache")
file(MAKE_DIRECTORY ${CACHE_DIR})

if(NOT EXISTS "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake"
        "${CMAKE_BINARY_DIR}/cmake/CPM.cmake"
    )
endif()
include(${CMAKE_BINARY_DIR}/cmake/CPM.cmake)

# raylib
CPMAddPackage(
    NAME raylib
    GITHUB_REPOSITORY raysan5/raylib
    GIT_TAG 5.5
    OPTIONS
        "BUILD_EXAMPLES OFF"
        "BUILD_GAMES OFF"
        "BUILD_SHARED_LIBS ON"
)

# raylib-nuklear
CPMAddPackage(
    NAME raylib_nuklear
    GITHUB_REPOSITORY RobLoach/raylib-nuklear
    GIT_TAG v5.5.1
    OPTIONS
        "RAYLIB_NUKLEAR_BUILD_EXAMPLES OFF"
)

# raygui
CPMAddPackage(
    NAME raygui
    GITHUB_REPOSITORY raysan5/raygui
    GIT_TAG 4.0
    DOWNLOAD_ONLY YES
)

# enet
CPMAddPackage(
    NAME enet
    GITHUB_REPOSITORY zpl-c/enet
    GIT_TAG v2.6.2
    DOWNLOAD_ONLY YES
)

# luajit
CPMAddPackage(
    NAME luajit
    GITHUB_REPOSITORY openresty/luajit2
    GIT_TAG v2.1-20250117
    DOWNLOAD_ONLY YES
)

# flecs
CPMAddPackage(
    NAME flecs
    GITHUB_REPOSITORY SanderMertens/flecs
    GIT_TAG v4.0.5
    OPTIONS
        "FLECS_STATIC OFF"
        "FLECS_SHARED ON"
)

if(luajit_ADDED)
    set(LUAJIT_SRC_DIR ${luajit_SOURCE_DIR})
    set(LUAJIT_DLL_SRC "${LUAJIT_SRC_DIR}/src/lua51.dll")
    set(LUAJIT_LIB_SRC "${LUAJIT_SRC_DIR}/src/lua51.lib")
    set(LUAJIT_DLL "${CACHE_DIR}/lua51.dll")
    set(LUAJIT_LIB "${CACHE_DIR}/lua51.lib")

    if(NOT EXISTS ${LUAJIT_DLL} OR NOT EXISTS ${LUAJIT_LIB} OR "${LUAJIT_SRC_DIR}/src/luajit.c" IS_NEWER_THAN ${LUAJIT_DLL})
        set(VS_VCVARSALL "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat")
        add_custom_command(
            OUTPUT ${LUAJIT_DLL_SRC} ${LUAJIT_LIB_SRC}
            COMMAND cmd /c "call \"${VS_VCVARSALL}\" x64 && cd /d ${LUAJIT_SRC_DIR}/src && call ${LUAJIT_SRC_DIR}/src/msvcbuild.bat dll"
            DEPENDS ${LUAJIT_SRC_DIR}/src/luajit.c
            WORKING_DIRECTORY ${LUAJIT_SRC_DIR}/src
            COMMENT "Building LuaJIT shared library with MSVC"
        )
        add_custom_command(
            OUTPUT ${LUAJIT_DLL} ${LUAJIT_LIB}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LUAJIT_DLL_SRC} ${LUAJIT_DLL}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LUAJIT_LIB_SRC} ${LUAJIT_LIB}
            DEPENDS ${LUAJIT_DLL_SRC} ${LUAJIT_LIB_SRC}
            COMMENT "Caching LuaJIT build artifacts"
        )
        add_custom_target(luajit_build DEPENDS ${LUAJIT_DLL} ${LUAJIT_LIB})
    endif()

    add_library(luajit SHARED IMPORTED)
    set_target_properties(luajit PROPERTIES
        IMPORTED_LOCATION "${LUAJIT_DLL}"
        IMPORTED_IMPLIB "${LUAJIT_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${LUAJIT_SRC_DIR}/src"
    )
    if(TARGET luajit_build)
        add_dependencies(luajit luajit_build)
    endif()
endif()

# mimalloc (Configured as DLL)
CPMAddPackage(
    NAME mimalloc
    GITHUB_REPOSITORY microsoft/mimalloc
    GIT_TAG v2.2.2
    OPTIONS
        "MI_BUILD_TESTS OFF"
        "MI_BUILD_EXAMPLES OFF"
        "MI_BUILD_SHARED ON"
)

# Check and configure raylib.dll
set(RAYLIB_DLL_SRC "${raylib_BINARY_DIR}/raylib/Debug/raylib.dll")
set(RAYLIB_DLL "${CACHE_DIR}/raylib.dll")
if(NOT EXISTS ${RAYLIB_DLL} OR "${raylib_SOURCE_DIR}/src/raylib.h" IS_NEWER_THAN ${RAYLIB_DLL})
    add_custom_command(
        OUTPUT ${RAYLIB_DLL}
        COMMAND ${CMAKE_COMMAND} -E echo "Copying raylib.dll from ${RAYLIB_DLL_SRC} to ${RAYLIB_DLL}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RAYLIB_DLL_SRC} ${RAYLIB_DLL}
        DEPENDS raylib
        COMMENT "Caching raylib.dll from ${RAYLIB_DLL_SRC}"
    )
    add_custom_target(raylib_cache DEPENDS ${RAYLIB_DLL})
endif()

# Check and configure flecs.dll
set(FLECS_DLL_SRC "${flecs_BINARY_DIR}/Debug/flecs.dll")
set(FLECS_DLL "${CACHE_DIR}/flecs.dll")
if(NOT EXISTS ${FLECS_DLL} OR "${flecs_SOURCE_DIR}/flecs.c" IS_NEWER_THAN ${FLECS_DLL})
    add_custom_command(
        OUTPUT ${FLECS_DLL}
        COMMAND ${CMAKE_COMMAND} -E echo "Copying flecs.dll from ${FLECS_DLL_SRC} to ${FLECS_DLL}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FLECS_DLL_SRC} ${FLECS_DLL}
        DEPENDS flecs
        COMMENT "Caching flecs.dll from ${FLECS_DLL_SRC}"
    )
    add_custom_target(flecs_cache DEPENDS ${FLECS_DLL})
endif()

# Check and configure mimalloc.dll (Updated to mimalloc-debug.dll)
set(MIMALLOC_DLL_SRC "${mimalloc_BINARY_DIR}/Debug/mimalloc-debug.dll")  # Updated to match actual output
set(MIMALLOC_DLL "${CACHE_DIR}/mimalloc-debug.dll")  # Keep the debug suffix for consistency
if(NOT EXISTS ${MIMALLOC_DLL} OR "${mimalloc_SOURCE_DIR}/src/alloc.c" IS_NEWER_THAN ${MIMALLOC_DLL})
    add_custom_command(
        OUTPUT ${MIMALLOC_DLL}
        COMMAND ${CMAKE_COMMAND} -E echo "Copying mimalloc-debug.dll from ${MIMALLOC_DLL_SRC} to ${MIMALLOC_DLL}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MIMALLOC_DLL_SRC} ${MIMALLOC_DLL}
        DEPENDS mimalloc
        COMMENT "Caching mimalloc-debug.dll from ${MIMALLOC_DLL_SRC}"
    )
    add_custom_target(mimalloc_cache DEPENDS ${MIMALLOC_DLL})
endif()

set(LUA_MODULE_SRC_FILES
    src/lua_enet.c
    src/lua_raylib.c 
    src/lua_raymath.c 
    src/lua_raygui.c
    src/lua_flecs.c
    # src/flecs_module.c
    # src/flecs_raylib.c
    # src/flecs_raygui.c
)

set(FLECS_MODULE_SRC_FILES
    # src/lua_enet.c
    # src/lua_raylib.c 
    # src/lua_raymath.c 
    # src/lua_raygui.c
    # src/lua_flecs.c
    src/flecs_module.c
    src/flecs_raylib.c
    src/flecs_raygui.c
    # src/impl_dk_console.c
    src/dk_ui.c
    src/dk_console.c
    src/flecs_dk_console.c
)
set(app_lua main_luajit)
# MAIN
add_executable(${app_lua} 
    # src/main.c
    # src/raylib_collision.c
    # src/raylib_collision_fps.c
    src/main_luajit.c
    ${LUA_MODULE_SRC_FILES}
)

target_compile_definitions(${app_lua} PUBLIC
  # -DENET_IMPLEMENTATION
  # -DDK_CONSOLE_IMPLEMENTATION
  # -DDK_CONSOLE_EXT_COMMAND_IMPLEMENTATION
  -DRAYGUI_IMPLEMENTATION
  -D_CRT_SECURE_NO_WARNINGS
)

if(TARGET luajit_build)
    add_dependencies(${app_lua} luajit_build)
endif()
if(TARGET raylib_cache)
    add_dependencies(${app_lua} raylib_cache)
endif()
if(TARGET flecs_cache)
    add_dependencies(${app_lua} flecs_cache)
endif()
if(TARGET mimalloc_cache)
    add_dependencies(${app_lua} mimalloc_cache)
endif()

target_include_directories(${app_lua} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${raylib_SOURCE_DIR}/src
    ${luajit_SOURCE_DIR}/src
    ${raygui_SOURCE_DIR}/src
    ${enet_SOURCE_DIR}/include
    ${flecs_SOURCE_DIR}/include
    ${mimalloc_SOURCE_DIR}/include
    ${raylib_nuklear_SOURCE_DIR}/include
)

target_link_libraries(${app_lua} PRIVATE
    raylib
    flecs
    luajit
    mimalloc-static # working
    # mimalloc #not working
    # mimalloc-debug #not working
    # ${mimalloc_BINARY_DIR}/Debug/mimalloc-debug.lib #working
)

if(WIN32)
    target_link_libraries(${app_lua} PRIVATE ws2_32 winmm)
endif()

# Copy DLLs to output directory
set(RAYLIB_DLL_DEST "${CMAKE_BINARY_DIR}/Debug/raylib.dll")
if(NOT EXISTS ${RAYLIB_DLL_DEST})
  add_custom_command(TARGET ${app_lua} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${RAYLIB_DLL}"
          "$<TARGET_FILE_DIR:${app_lua}>/raylib.dll"
      COMMENT "Copying raylib.dll to output directory"
  )
endif()

set(LUAJIT_DLL_DEST "${CMAKE_BINARY_DIR}/Debug/lua51.dll")
if(NOT EXISTS ${LUAJIT_DLL_DEST})
  add_custom_command(TARGET ${app_lua} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${LUAJIT_DLL}"
          "$<TARGET_FILE_DIR:${app_lua}>/lua51.dll"
      COMMENT "Copying lua51.dll to output directory"
  )
endif()

set(FLECS_DLL_DEST "${CMAKE_BINARY_DIR}/Debug/flecs.dll")
# message(STATUS "FLECS_DLL_DEST >>> ${FLECS_DLL_DEST}")
if(NOT EXISTS ${FLECS_DLL_DEST})
  # message(STATUS "NOT FOUND FLECS>>>>>>>>")
  add_custom_command(TARGET ${app_lua} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${FLECS_DLL}"
          "$<TARGET_FILE_DIR:${app_lua}>/flecs.dll"
      COMMENT "Copying flecs.dll to output directory"
  )
endif()

set(MIMALLOC_DLL_DEST "${CMAKE_BINARY_DIR}/Debug/mimalloc-debug.dll")
if(NOT EXISTS "${MIMALLOC_DLL_DEST}")
  add_custom_command(TARGET ${app_lua} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${MIMALLOC_DLL}"
          "$<TARGET_FILE_DIR:${app_lua}>/mimalloc-debug.dll"  # Updated to match cached name
      COMMENT "Copying mimalloc-debug.dll to output directory"
  )
endif()

add_custom_command(TARGET ${app_lua} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/script.lua"
        "$<TARGET_FILE_DIR:${app_lua}>/script.lua"
    COMMENT "Copying script.lua to output directory"
)

#================================================
add_executable(main_flecs_module
  src/main_flecs_module.c
  ${FLECS_MODULE_SRC_FILES}
)

target_compile_definitions(main_flecs_module PUBLIC
  # -DENET_IMPLEMENTATION
  -D_CRT_SECURE_NO_WARNINGS
  -DRAYGUI_IMPLEMENTATION
)

target_link_libraries(main_flecs_module PRIVATE
    raylib
    flecs
    luajit
    # mimalloc
    mimalloc-static # working
)

target_include_directories(main_flecs_module PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${raylib_SOURCE_DIR}/src
    ${luajit_SOURCE_DIR}/src
    ${raygui_SOURCE_DIR}/src
    ${enet_SOURCE_DIR}/include
    ${flecs_SOURCE_DIR}/include
    ${mimalloc_SOURCE_DIR}/include
    ${raylib_nuklear_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(main_flecs_module PRIVATE
        ws2_32
        winmm
    )
endif()

# Copy entire assets folder (including subdirectories) to build output
set(ASSETS_SRC_DIR "${CMAKE_SOURCE_DIR}/resources")  # Use quotes and forward slashes
# set(ASSETS_DEST_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/resources")
set(ASSETS_DEST_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>/resources")

# Check if assets directory exists
if(NOT EXISTS "${ASSETS_SRC_DIR}")
    message(WARNING "Assets directory '${ASSETS_SRC_DIR}' does not exist. Creating empty directory in build output.")
    # file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/resources)
endif()

# Add custom command to copy entire assets directory
add_custom_command(TARGET main_flecs_module POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DEST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_SRC_DIR}" "${ASSETS_DEST_DIR}"
    COMMENT "Copying assets directory to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/resources"
)

set(DK_CONSOLE_SRC 
  src/dk_ui.c
  src/dk_console.c
)

# set(BUILD_ALL_EXAMPLES OFF CACHE BOOL "Build all examples")
# #set(BUILD_ALL_EXAMPLES OFF)
set(BUILD_ALL_EXAMPLES ON)

set(examples
  # src/main_flecs.c
  # src/main_flecs_render.c
  # src/main_luajit.c # load script
  # src/main_raylib.c
  # src/raylib_collision_fps.c
  # src/raylib_collision.c
  # src/main_flecs_module.c
  # src/test.c
  # src/raylib_transform3d.c
  # src/raylib3d_flecs_transform_hierarchy06.c
  src/flecs_test.c
  # src/raylib_capture_input_camera.c
  # examples/c/raylib/raylib_toggle_camera_mode01.c
  # examples/c/raylib/raylib_dk_console.c
  # examples/c/flecs/flecs_delta_time.c
  
)
# message(STATUS "BUILD_ALL_EXAMPLES >> ${BUILD_ALL_EXAMPLES}")

foreach(example_source ${examples})
    #message(STATUS "BUILD_ALL_EXAMPLES >> ${BUILD_ALL_EXAMPLES}")
    get_filename_component(example_name ${example_source} NAME_WE)
    string(REPLACE ".c" "" example_name ${example_name})
    
    # Create individual option, default ON
    option(BUILD_${example_name} "Build ${example_name} example" ON)
    
    # Build only if BUILD_ALL_EXAMPLES is ON and individual option is ON
    # If BUILD_ALL_EXAMPLES is OFF, nothing builds regardless of individual options
    if(BUILD_ALL_EXAMPLES)
        if(BUILD_${example_name})
            add_executable(
              ${example_name} 
              ${example_source}
              ${DK_CONSOLE_SRC}
              # ${LUA_MODULE_SRC_FILES}
            )

            target_compile_definitions(${example_name} PUBLIC
              # -DENET_IMPLEMENTATION
              # -DRAYGUI_IMPLEMENTATION
              -D_CRT_SECURE_NO_WARNINGS
            )
            
            target_link_libraries(${example_name} PRIVATE
                raylib
                flecs
                luajit
                # mimalloc
                mimalloc-static # working
            )

            target_include_directories(${example_name} PRIVATE
                ${CMAKE_SOURCE_DIR}/include
                ${raylib_SOURCE_DIR}/src
                ${luajit_SOURCE_DIR}/src
                ${raygui_SOURCE_DIR}/src
                ${enet_SOURCE_DIR}/include
                ${flecs_SOURCE_DIR}/include
                ${mimalloc_SOURCE_DIR}/include
                ${raylib_nuklear_SOURCE_DIR}/include
            )
            
            if(WIN32)
                target_link_libraries(${example_name} PRIVATE
                    ws2_32
                    winmm
                )
            endif()
            
            message(STATUS "Added example: ${example_name}")
        endif()
    endif()
endforeach()

