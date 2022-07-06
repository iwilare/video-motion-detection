#!/bin/sh

# Start all benchmarks with 4 repetitions up to 64 threads
bin/measurements     benchmark 4 -n 64 > data/measurements.csv
bin/sequential       benchmark 4 -n 64 > data/sequential.csv
bin/fastflow         benchmark 4 -n 64 > data/fastflow.csv
bin/threads          benchmark 4 -n 64 > data/threads.csv
bin/threads_affinity benchmark 4 -n 64 > data/threads_affinity.csv
bin/omp              benchmark 4 -n 64 > data/omp.csv
