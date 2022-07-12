#!/bin/sh

# Measurements on the individual stages of the program, with 5 repetitions.
bin/measurements videos/test_big.mp4   5
bin/measurements videos/test_mid.mp4   5
bin/measurements videos/test_small.mp4 5

# <number of iterations> <print read csv> <do work>

# Fast successive read mode using third flag=1, single run
bin/measurements videos/test_big.mp4   1 1 1 > data/fast_read_test_big.csv
bin/measurements videos/test_mid.mp4   1 1 1 > data/fast_read_test_mid.csv
bin/measurements videos/test_small.mp4 1 1 1 > data/fast_read_test_small.csv

# Read mode with grey and blur work using third flag=1, single run
bin/measurements videos/test_big.mp4   1 1 0 > data/work_read_test_big.csv
bin/measurements videos/test_mid.mp4   1 1 0 > data/work_read_test_mid.csv
bin/measurements videos/test_small.mp4 1 1 0 > data/work_read_test_small.csv
