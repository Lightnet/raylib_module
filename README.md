# raylib_module

# License:
- MIT License - Free to use, modify, and share!

# Table of Contents  
 * [Information](#information)
 * [Design Goal](#design-goals)
 * [Libraries Used](#libraries-used)
 * [Lua Script](#lua-script)
 * [Flecs Module](#flecs-module)
 * [Project files](#project-files)
 * [Design](#design)
 * [CMakeLists.txt](#cmakeLists.txt)
 * [Main Files](#main-files)
 * [How It Works](#how-it-works)
 * [What You Need](#What-You-Need)
 * [Building It](#building-it)
 * [Lua Script Tips](#lua-script-tips)
 * [Troubleshooting](#troubleshooting)
 * [Credits](#Credits)

---
# Information:
  Work in progress build.

  By using raylib, raygui, enet, raymath, LuaJIT and other libraries. 

  There are two build for raylib for module and lua script.

## Design Goals:
- Run Lua scripts as the entry point instead of rebuilding a binary each time.
- Keep C-to-Lua and Lua-to-C data (like colors, vectors) in a clear, matching format.
- raylib, flecs and module design to keep things simple.

---

# Libraries Used
- [x] raylib 5.5: Core library for graphics, input, and more. Includes raymath (work-in-progress Lua wrapper).
- [x] raygui 4.0: Simple GUI library for raylib (work-in-progress Lua wrapper).
- [x] enet 2.6.2: Networking library (DLL support is buggy right now).
- [x] LuaJIT 2.1 Fast Lua runtime to run scripts.
  - [x] Lua 5.1
- [x] flecs 4.0.5
- [ ] raylib nuklear 5.5.1
- [ ] mimalloc 2.2.2 

---

# Lua Script:
  Lua script for designed to be simple and avoid recompiling C code every time—just write Lua and run!

## Why?
- Faster Workflow: Edit Lua scripts and run them directly with raylib_luajit.exe.
- Learning Tool: A starting point to play with raylib, networking, and GUI in Lua.

## Current Status:
- It works! See demo.lua for a test with graphics, networking, and GUI.
- Still debugging and adding features (e.g., more raymath functions).

# Flecs Module:
  This project is testing module design. The other is flecs turn into module.

---

# Project Files:
  There are three main files for module and normal test build.

 * main_luajit.c - for LuaJIT for script lua.
 * main_raylib.c - normal raylib with c.
 * main_flecs_module.c - Flecs build test.
```
project/
- docs
- examples
- include
  - flecs_module.h                                // module
  - flecs_raylib.h                                // module
  - lua_enet.h                                    // lua
  - lua_flecs_comps.h                             // lua
  - lua_flecs.h                                   // lua
  - lua_raygui.h                                  // lua
  - lua_raylib.h                                  // lua
  - lua_raymath.h                                 // lua
  - lua_utils.h                                   // lua
- src
  - flecs_module.c                                // module
  - flecs_raylib.c                                // module
  - flecs_test.c                                  // test
  - lua_enet.c                                    // lua
  - lua_flecs.c                                   // lua
  - lua_raygui.c                                  // lua
  - lua_raylib.c                                  // lua
  - lua_raymath.c                                 // lua
  - main_flecs_module.c                           // main module
  - main_flecs_render.c                           // main render test
  - main_flecs.c                                  // main flecs test
  - main_luajit.c                                 // main lua
  - main_raylib.c                                 // main test
  - raylib_collision_fps.c                        // main test
  - raylib_collision.c                            // main test
  - raylib3d_flecs_transform_hierarchy06.c        // main transform 3d test
- build.bat //config cmake and compile project VS2022
- run.bat // for raylib lua script
- runm.bat //for raylib flecs module run test
- runt.bat //for raylib3d flecs transform hierarchy 06  test
```
  Note the batch script may change for testing builds.

# Design:
  Well working on module some degree to break apart to handle some libs to make it easy or hard depend on how to code correctly on the builds.

  Needed normal and module to test the correct way to handle input and collision later build tests.
## lua:
- [x] simple window frame test
- [x] draw text

## flecs module:
- [ ] raylib
    - [x] base window
    - [ ] base model
        - [x] clean up
    - [ ] loading model
    - [x] transform 3d hierarchy
    - [ ] loading screen
- [x] camera mode:
    - [x] player camera
    - [x] free camera
    - [ ] debug
- [x] player 
    - [ ] Jump
    - [ ] collision
    - [x] movement
    - [ ] health 
    - [ ] damage
- [ ] dev console
    - [x] echo text
    - [x] " ` " < (backtick) or " ~ " < (tilde) = key toggle show dev console.
    - [x] default key for Arrow up and down scroll from dk_console. Arrow Down key is up scroll. Arrow Up key is down scroll.
- [ ] physics ?


## CMakeLists.txt
  Current the testing has not been config just prototyping the build.

```
# MAIN
add_executable(${PROJECT_NAME} 
  # src/main.c
  # src/raylib_collision.c
  # src/raylib_collision_fps.c
  # src/main_flecs.c
  # src/main_raylib.c
  src/main_luajit.c
  ${LUA_MODULE_SRC_FILES}
)
```

```
set(examples
  # src/main_flecs.c
  # src/main_flecs_render.c
  # src/main_luajit.c # load script
  # src/main_raylib.c
  # src/raylib_collision_fps.c
  # src/raylib_collision.c
  src/main_flecs_module.c
)
```
Example files current being tested. Note that one file can be active due to over lap function name conflicts.

## Main Files:
 - src/main_luajit.c (work in progress, lua script)
 - src/main_flecs_module.c (work in progress, module design)

---

# How It Works

We use CMake to build one binary (raylib_luajit.exe) that loads Lua scripts. There are two ways to use raylib in Lua:
1. Lua Wrapper (Recommended): Pre-built C-to-Lua bindings (e.g., rl.InitWindow).
2. FFI (Alternative): Load raylib’s C functions directly from Lua (needs a DLL).

---

# What You Need
- Visual Studio 2022: For building LuaJIT (uses a Makefile).
- CMake: To generate the build files and compile everything.

# Building It
1. Install Requirements:
    - Visual Studio 2022 (Community edition works).
    - CMake (download from cmake.org or install via package manager).

2. Clone the Repo:
    bash
    ```bash
    git clone https://github.com/Lightnet/raylib_module.git
    cd raylib_module
    ```
    
3. Build with CMake:
    powershell
    ```powershell
    mkdir build
    cd build
    cmake .. -G "Visual Studio 17 2022" -A x64
    cmake --build . --config Debug
    ```

4. Run:
    - Server: .\Debug\raylib_luajit.exe ..\..\server.lua
    - Client: .\Debug\run.bat (runs demo.lua).
    
---

# Lua Script Tips
 work in progress.

## Colors
- Predefined: Use rl.RAYWHITE, rl.RED, rl.GREEN, etc.
- Custom: Pass a table {r, g, b, a} (e.g., {128, 0, 128, 255} for purple).

### Check demo.lua for a working example with:
- Moving rectangle (raylib + raymath).
- Networking (enet).
- GUI button, label, checkbox (raygui).
---

# Troubleshooting
- LuaJIT Build: Run in a VS2022 Developer PowerShell for msvcbuild.bat to work.
- CMake Errors: Ensure all libs (raylib, enet, etc.) downloaded via CPM (CMake handles this).
- Black Screen: Rebuild fully (Remove-Item -Recurse -Force * in build/ folder).

---

# Credits:
- raylib-lua: Inspired setup ([https://github.com/raysan5/raylib-lua](https://github.com/raysan5/raylib-lua)).
	- AI Help: Used Grok (xAI) to debug CMake and wrappers.
- flecs:
  - people from discord server.
- raylib github
- https://github.com/dkvilo/dk_console
    - very simple dev console in raylib game/engine
    - rework to match raylib 5.5
- https://kenney.nl/assets/kenney-fonts
    - freetype font

