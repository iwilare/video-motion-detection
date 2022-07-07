#!/bin/sh

# Start all benchmarks with 4 repetitions up to 75 threads
mkdir data
bin/sequential        benchmark 4 -n 1  | tee data/sequential.csv
bin/fastflow          benchmark 4 -n 33 | tee data/fastflow.csv
bin/fastflow_blocking benchmark 4 -n 33 | tee data/fastflow_blocking.csv
bin/threads           benchmark 4 -n 33 | tee data/threads.csv
bin/threads_affinity  benchmark 4 -n 33 | tee data/threads_affinity.csv
bin/omp               benchmark 4 -n 33 | tee data/omp.csv
