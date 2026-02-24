#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$( realpath "$SCRIPT_DIR/../../../.." )"


# Compiler and flags
CXX=g++
CC=gcc
CXXFLAGS="-I. -g -O0 -I$PROJECT_ROOT/external/systemc-2.3.3/include -I$PROJECT_ROOT/external/systemc-ams-2.3.4/include -std=c++17 "
CFLAGS="-I. -g -O0  -I$PROJECT_ROOT/external/systemc-2.3.3/include -I$PROJECT_ROOT/external/systemc-ams-2.3.4/include"
LDFLAGS="-L$PROJECT_ROOT/external/systemc-2.3.3/lib-linux64 -L$PROJECT_ROOT/external/systemc-ams-2.3.4/lib-linux64 \
         -Wl,-rpath,$PROJECT_ROOT/external/systemc-2.3.3/lib-linux64:$PROJECT_ROOT/external/systemc-ams-2.3.4/lib-linux64 \
         -lsystemc-ams -lsystemc -lm"

# Build directory
BUILD_DIR=build
TARGET=$BUILD_DIR/out
OBJS="$BUILD_DIR/main.o $BUILD_DIR/hello_tdf.o $BUILD_DIR/print_sink.o "

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
        # Compile all relevant .cpp files
        compile_cpp src/main.cpp $BUILD_DIR/main.o
        compile_cpp src/hello_tdf.cpp $BUILD_DIR/hello_tdf.o
        compile_cpp src/print_sink.cpp $BUILD_DIR/print_sink.o
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