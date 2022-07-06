#!/bin/sh

# Measurements on the individual stages of the program, with 10 repetitions
bin/measurements videos/test_big.mp4   -n 10
bin/measurements videos/test_mid.mp4   -n 10
bin/measurements videos/test_small.mp4 -n 10
