#!/bin/bash

# Create the build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Run cmake to configure the project and generate the Makefiles
cmake -B build

# Build the project
cmake --build build --target Chess

# Change directory to the build folder and run the executable
cd build && ./Chess
