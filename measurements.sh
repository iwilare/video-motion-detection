#!/bin/sh

# Measurements on the individual stages of the program, with 5 repetitions
#bin/measurements videos/test_big.mp4   -n 5
#bin/measurements videos/test_mid.mp4   -n 5
#bin/measurements videos/test_small.mp4 -n 5

# Fast successive read mode using -n=1
bin/measurements videos/test_big.mp4   -n 1 > data/fast_read_test_big.csv
bin/measurements videos/test_mid.mp4   -n 1 > data/fast_read_test_mid.csv
bin/measurements videos/test_small.mp4 -n 1 > data/fast_read_test_small.csv

# Read mode with grey and blur work using -n=2
bin/measurements videos/test_big.mp4   -n 2 > data/work_read_test_big.csv
bin/measurements videos/test_mid.mp4   -n 2 > data/work_read_test_mid.csv
bin/measurements videos/test_small.mp4 -n 2 > data/work_read_test_small.csv
