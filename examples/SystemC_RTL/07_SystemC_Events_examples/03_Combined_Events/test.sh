#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( realpath "$SCRIPT_DIR/../../../.." )"

# Compiler and flags
CXX=g++
CC=gcc
CXXFLAGS="-I. -g -O0 -Iinclude -I$PROJECT_ROOT/external/systemc-3.0.2/install/include -std=c++17"
CFLAGS="-I. -g -O0 -Iinclude -I$PROJECT_ROOT/external/systemc-3.0.2/install/include"
LDFLAGS="-L. -L$PROJECT_ROOT/external/systemc-3.0.2/install/lib -Wl,-rpath,$PROJECT_ROOT/external/systemc-3.0.2/install/lib -lsystemc -lm"

# Build directory
BUILD_DIR=build
TARGET=$BUILD_DIR/out
OBJS=" $BUILD_DIR/combined.o $BUILD_DIR/main.o "

# Functions
compile_cpp() {
    echo "Compiling C++ file $1 to $2..."
    $CXX $CXXFLAGS -c $1 -o $2
    if [ $? -ne 0 ]; then
        echo "Error compiling $1"
        exit 1
    fi
}

compile_c() {
    echo "Compiling C file $1 to $2..."
    $CC $CFLAGS -c $1 -o $2
    if [ $? -ne 0 ]; then
        echo "Error compiling $1"
        exit 1
    fi
}

link() {
    echo "Linking object files to create $TARGET..."
    $CXX $OBJS -o $TARGET $LDFLAGS
    if [ $? -ne 0 ]; then
        echo "Error during linking"
        exit 1
    fi
}

clean() {
    echo "Cleaning up..."
    rm -rf $BUILD_DIR
}

# Main script
case "$1" in
    all)
        # Create build directory if it doesn't exist
        if [ ! -d "$BUILD_DIR" ]; then
            mkdir $BUILD_DIR
        fi
        # Compile all relevant .cpp and .c files
        compile_cpp src/combined.cpp $BUILD_DIR/combined.o
        compile_cpp src/main.cpp $BUILD_DIR/main.o
        link
        ;;
    run)
        if [ -f "$TARGET" ]; then
            echo "Running the executable $TARGET..."
            ./$TARGET
        else
            echo "Executable not found. Build the project first by running '$0 all'."
        fi
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {all|run|clean}"
        exit 1
        ;;
esac
