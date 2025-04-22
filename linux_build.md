# Information:
  Work in progress. It need to be refine later.

# Linux Alpine Android:
  Have choice this since it very minimal. Only need to run virutal desktop.
  Have about 1.3 GB less. It depend on used.

  This is very simple window check to render opengl.

## Required:
- gcc  (tool)
- g++  (tool)
- make (tool)
- cmake (tool)
- libx11-dev (lib for raylib)
- libxrandr-dev (lib for raylib)
- libinerama-dev (lib for raylib)
- libcursor-dev (lib for raylib)
- libxi-dev (lib for raylib)
- glu-dev (lib for raylib)

build.sh
```sh
#!/bin/sh
 
SOURCE_DIR="."
BUILD_DIR="build"
 
mkdir -p "$BUILD_DIR"
 
cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" \
  -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug
 
cmake --build "$BUILD_DIR"
```

```
chmod +x build.sh
```
allow permission to run script.