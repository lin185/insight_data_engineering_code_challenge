#!/bin/sh

rm ./wc_output/wc_result.txt ./wc_output/med_result.txt

gcc -o ./src/wordcount ./src/wordcount.c
./src/wordcount ./wc_input ./wc_output/wc_result.txt

#echo

gcc -o ./src/running_median ./src/running_median.c
./src/running_median ./wc_input ./wc_output/med_result.txt
