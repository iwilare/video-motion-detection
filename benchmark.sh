#!/bin/sh

make &&
> results_small.csv
echo 'type,n_threads,time' >> results_small.csv
