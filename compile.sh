#!/bin/bash

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

if [ "$1" == "clean" ]; then
    rm -rf build
fi

if [ ! -d "./build" ]; then
    mkdir build
    cd build
    cmake .. -G "Visual Studio 15 Win64"
    cd ../
fi

TYPE="Release"

if [ "$1" == "debug" ] || [ "$2" == "debug" ] ; then
    TYPE="Debug"
fi

cmake --build build --config "$TYPE"

if [[ $? > 0 ]]; then
    exit 1
fi

GLSL="src/*.glsl"
DLL="lib/*.dll"

mkdir -p bin/Release/assets
mkdir -p bin/Debug/assets

cp $GLSL bin/Release/assets
cp $GLSL bin/Debug/assets
cp $DLL bin/Release
cp $DLL bin/Debug

cd bin/Release
./main.exe
cd ../..
