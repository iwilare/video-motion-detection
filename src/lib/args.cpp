#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

// Parse and get the command line arguments.
void get_arguments(int argn, char *argc[], string* filename, float* detection_percentage, float* difference_threshold, size_t* n_workers, size_t* benchmark) {
    // Check command line arguments
    if(argn <= 1) {
        cout << "Usage: " << argc[0] << " <filename> [options]\n";
        cout << "[options]:\n";
        cout << "  [-d detection_percentage]:\n";
        cout << "    percentage of differing pixels  (default=0.05)\n";
        cout << "\n";
        cout << "  [-t greyscale_difference_threshold]:\n";
        cout << "    greyscale difference to consider a pixel as changed (default=0.05)\n";
        cout << "\n";
        cout << "  [-n number_of_workers]:\n";
        cout << "    total parallelism degree/resources used in the application\n";
        cout << "    (default=<parallelism degree provided by STL)\n";
        cout << "\n";
        cout << "Example:\n";
        cout << "  " << argc[0] << " videos/test_mid.mp4 -d 0.02 -t 0.3 -n 4";
        cout << "Benchmark mode:\n";
        cout << "  " << argc[0] << " benchmark <filename> <n_repetitions> [-n max_par_degree] [options]\n";
        exit(0);
    }
    int i = 1;

    // Read the filename parameter or the benchmark keyword

    if(strcmp(argc[i], "benchmark") == 0) {
        i++;
        if(i >= argn) {
            cout << "Error: benchmark requires a parameter." << endl;
            exit(1);
        }
        *benchmark = atoi(argc[i++]);
        if(*benchmark <= 0) {
            cout << "Error: benchmark must be a valid integer greater than 0." << endl;
            exit(1);
        }
    } else {
        *filename = argc[i++];
        // Check that the file exists
        ifstream file(*filename);
        if(!file.good()) {
            cout << "Error: file " << *filename << " cannot be opened." << endl;
            exit(1);
        }
    }

    for(; i < argn; i++) {
        if(strcmp(argc[i], "-d") == 0) {
            i++;
            if(i >= argn) {
                cout << "Error: -d requires a parameter." << endl;
                exit(1);
            }
            *detection_percentage = atof(argc[i]);
            if(!(0.0 <= *detection_percentage && *detection_percentage <= 1.0)) {
                cout << "Error: detection_percentage must be a valid float percentage between 0 and 1." << endl;
                exit(1);
            }
        } else if(strcmp(argc[i], "-t") == 0) {
            i++;
            if(i >= argn) {
                cout << "Error: -t requires a parameter." << endl;
                exit(1);
            }
            *difference_threshold = atof(argc[i]);
            if(!(0.0 <= *difference_threshold && *difference_threshold <= 1.0)) {
                cout << "Error: difference_threshold must be a valid float percentage between 0 and 1." << endl;
                exit(1);
            }
        } else if(strcmp(argc[i], "-n") == 0) {
            i++;
            if(i >= argn) {
                cout << "Error: -n requires a parameter." << endl;
                exit(1);
            }
            *n_workers = atoi(argc[i]);
            if(*n_workers <= 0) {
                cout << "Error: number_of_workers must be a valid integer greater than 0." << endl;
                exit(1);
            }
        } else {
            cout << "Error: unrecognized argument " << argc[i] << endl;
            exit(1);
        }
    }
}
