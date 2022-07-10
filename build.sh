#!/bin/bash

rm -rf build
mkdir build
g++-10 src/main.cpp -o build/main `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17 #remote
#g++ src/main.cpp -o build/main `pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17 #local
