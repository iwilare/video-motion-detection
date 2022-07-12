# Video motion detection

## Getting started

```bash
git clone https://github.com/iwilare/video-motion-detection
cd video-motion-detection
cmake .
make
```

Upon successful compilation the resulting executables can be found in `bin/`, and can be tested with the examples provided in `video/`.

### Usage

```
bin/threads <filename> [options]

[options]:
  [-d detection_percentage]:
    percentage of differing pixels  (default=0.05)

  [-t greyscale_difference_threshold]:
    greyscale difference to consider a pixel as changed (default=0.05)

  [-n number_of_workers]:
    total parallelism degree/resources used in the application (default=<parallelism degree provided by STL>)
```

Standard usage, outputting the number of motion frames over the total amount of frames along with an indication of the total microseconds taken. Arguments can be optional and the default values will be used, but must be provided in the given order

### Example

Test with 0.02 greyscale threshold difference, >=30% differing pixels, default H1 average kernel, using 4 as total number of workers (1 emitter + 3 workers):
```
bin/fastflow videos/test_mid.mp4 -d 0.02 -t 0.3 -n 4
```

```
> Motion frames over total video frames: (105 / 431)
> Total microseconds: 1695792
```

The kernel used in the processing can be specified with the `KERNEL` compilation flag, selecting between `H1,H2,H3,H4` or defining a new one in `src/lib/kernels.cpp`.

### Benchmark mode usage

```
bin/threads benchmark <filename> <n_repetitions> [-n max_par_degree] [options]
```

Starts the video detection program in benchmark mode. The files in the `video/` folder are opened, and the program tested with increasing parallelism degree up until the value provided with the `-n` flag. The execution times are averaged over `n_repetitions` executions, and the standard deviation is printed. This is useful to be redirected to a .csv file for analysis.

```
./benchmark.sh
```

All relevant benchmarks on all programs can be performed using the `./benchmark.sh` script.

## Measurements usage

Usage for the measurement program, used to obtain values for the sequential version and analyze the read operation in detail.

```
bin/measurements <n_repetitions> [print_read_times] [grey_blur_diff_workload]

  [print_read_times]: print the individual read times measured (default=0)

  [grey_blur_diff_workload]: enable the grey, blur, and difference workloads (default=1)
```

A concrete usage example can be found in the `./measurements.sh` script.

```
./measurements.sh
```

## Requirements

It is required for the following dependencies to be installed:

- FastFlow (3.0.1)
- OpenCV (4.4.5)
- OpenMP (4.1.2)

## File structure

A brief overview of the main files contained in the project is presented in the following tree.

```
+--videos/: contains the three example videos.
+--plot/: jupyter notebook for data visualization.
+--src/: source files.
| |
| +-- lib/: common files shared among all versions.
| | |
| | +-- args.cpp: command line arguments management.
| | +-- cpu_affinity.cpp: cpu affinity procedures.
| | +-- kernels.cpp: example kernel definitions.
| | +-- shared_queue.cpp: implementation of a thread-safe blocking queue.
| | +-- utimer.cpp: helper structure for time measurements.
| | +-- video_detection.cpp: main video detection logic and processing.
| | +-- main.cpp: common main structure for all implementations.
| +-- measurements.cpp: standalone file to measure the individual stages timings.
| +-- sequential.cpp: sequential implementation.
| +-- threads.cpp: main implementation with C++ STL threads.
| +-- fastflow.cpp: main implementation with FastFlow.
| +-- omp.cpp: main implementation with OpenMP.
| +-- ...: other versions.
+-- benchmark.sh: main file to run every benchmark, writes .csv to the "data/" folder.
+-- measurements.sh: script to measure sequential and read time on all videos.
```

## Visualization

All plots and visualizations used in the project report are reproducible using the Jupyter notebook provided in `plot/plot.ipynb`.
