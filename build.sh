#!/bin/bash
gcc -o main.bin main.c -ggdb -lSDL2 -lSDL2_image
if [ $? -ne 0 ]; then
    exit 1
fi
chmod +x main.bin
