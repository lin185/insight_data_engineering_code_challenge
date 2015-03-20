#!/bin/sh

gcc -o ./src/wordcount ./src/wordcount.c
echo "----------- Word Count Output ----------- "
./src/wordcount

echo