#!/bin/sh

# Start all benchmarks with 4 repetitions up to 35 threads
mkdir data
ps auxf > ps1.txt
bin/sequential               benchmark 4 -n 1  | tee data/sequential.csv
bin/fastflow                 benchmark 4 -n 35 | tee data/fastflow.csv
bin/fastflow_blocking_no_map benchmark 4 -n 35 | tee data/fastflow_blocking_no_map.csv
bin/fastflow_blocking        benchmark 4 -n 35 | tee data/fastflow_blocking.csv
bin/threads                  benchmark 4 -n 35 | tee data/threads.csv
bin/threads_pinned           benchmark 4 -n 35 | tee data/threads_pinned.csv
bin/omp                      benchmark 4 -n 35 | tee data/omp.csv
ps auxf > ps2.txt
bin/sequential               benchmark 4 -n 1  | tee data/sequential2.csv
bin/fastflow                 benchmark 4 -n 35 | tee data/fastflow2.csv
bin/fastflow_blocking_no_map benchmark 4 -n 35 | tee data/fastflow_blocking_no_map2.csv
bin/fastflow_blocking        benchmark 4 -n 35 | tee data/fastflow_blocking2.csv
bin/threads                  benchmark 4 -n 35 | tee data/threads2.csv
bin/threads_pinned           benchmark 4 -n 35 | tee data/threads_pinned2.csv
bin/omp                      benchmark 4 -n 35 | tee data/omp2.csv
ps auxf > ps3.txt
bin/sequential               benchmark 4 -n 1  | tee data/sequential3.csv
bin/fastflow                 benchmark 4 -n 35 | tee data/fastflow3.csv
bin/fastflow_blocking_no_map benchmark 4 -n 35 | tee data/fastflow_blocking_no_map3.csv
bin/fastflow_blocking        benchmark 4 -n 35 | tee data/fastflow_blocking3.csv
bin/threads                  benchmark 4 -n 35 | tee data/threads3.csv
bin/threads_pinned           benchmark 4 -n 35 | tee data/threads_pinned3.csv
bin/omp                      benchmark 4 -n 35 | tee data/omp3.csv
ps auxf > ps4.txt
bin/sequential               benchmark 4 -n 1  | tee data/sequential4.csv
bin/fastflow                 benchmark 4 -n 35 | tee data/fastflow4.csv
bin/fastflow_blocking_no_map benchmark 4 -n 35 | tee data/fastflow_blocking_no_map4.csv
bin/fastflow_blocking        benchmark 4 -n 35 | tee data/fastflow_blocking4.csv
bin/threads                  benchmark 4 -n 35 | tee data/threads4.csv
bin/threads_pinned           benchmark 4 -n 35 | tee data/threads_pinned4.csv
bin/omp                      benchmark 4 -n 35 | tee data/omp4.csv
