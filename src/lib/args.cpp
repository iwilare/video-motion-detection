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
        cout << "Usage: " << argc[0] << " <filename> [-d detection_percentage] [-t greyscale_difference_threshold] [-n number_of_workers]" << endl;
        cout << "     | " << argc[0] << " benchmark <n_repetitions> [-d detection_percentage] [-t greyscale_difference_threshold] [-n number_of_workers]" << endl;
        cout << "Example: " << argc[0] << " video.mp4 -d 0.3 -t 0.2 -n 4" << endl;
        exit(1);
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
