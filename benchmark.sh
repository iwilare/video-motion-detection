#!/bin/sh

# Start all benchmarks with 5 repetitions up to 35 threads
mkdir data
bin/sequential               benchmark 5 -n 1  | tee data/sequential.csv
bin/fastflow                 benchmark 5 -n 35 | tee data/fastflow.csv
bin/fastflow_blocking_no_map benchmark 5 -n 35 | tee data/fastflow_blocking_no_map.csv
bin/fastflow_blocking        benchmark 5 -n 35 | tee data/fastflow_blocking.csv
bin/threads                  benchmark 5 -n 35 | tee data/threads.csv
bin/threads_pinned           benchmark 5 -n 35 | tee data/threads_pinned.csv
bin/omp                      benchmark 5 -n 35 | tee data/omp.csv
