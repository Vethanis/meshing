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

mkdir -p bin/Release/assets
mkdir -p bin/Debug/assets

cp src/*.glsl bin/Release/assets
cp src/*.glsl bin/Debug/assets
cp lib/*.dll bin/Release
cp lib/*.dll bin/Debug

cd bin/Release
./main.exe
cd ../..
