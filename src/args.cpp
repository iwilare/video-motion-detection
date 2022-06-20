#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

// Parse and get the command line arguments.
void get_arguments(int argn, char *argc[], string* filename, float* difference_threshold, float* detection_percentage, size_t* n_workers, bool* thread_affinity) {
    // Check command line arguments
    if(!(2 <= argn && argn <= 5)) {
        cout << "Usage: " << argc[0] << " <filename> [detection percentage] [number of workers] [--affinity]" << endl;
        exit(1);
    }

    // Read the filename parameter
    *filename = argc[1];
    // Check that the file exists
    ifstream file(*filename);
    if(!file.good()) {
        cout << "Error: file " << *filename << " cannot be opened." << endl;
        exit(1);
    }

    // Read the threshold parameter
    if(argn >= 2) {
        *difference_threshold = atoi(argc[2]);
        if(*difference_threshold <= 0) {
            cout << "Error: difference_threshold must be a valid percentage between 0 and 1." << endl;
            exit(1);
        }
    }

    // Read the optional detection_percentage parameter
    if(argn >= 3) {
        *detection_percentage = atof(argc[3]);
        if(!(0.0 <= *detection_percentage && *detection_percentage <= 1.0)) {
            cout << "Error: detection_percentage must be a valid float percentage between 0 and 1." << endl;
            exit(1);
        }
    }

    // Read the workers parameter
    if(argn >= 4) {
        *n_workers = atoi(argc[4]);
        if(*n_workers <= 0) {
            cout << "Error: the number of workers must be a valid number greater than zero." << endl;
            exit(1);
        }
    }

    // Read the affinity option
    if(argn >= 5) {
        if(strcmp(argc[5], "--affinity") == 0)
            *thread_affinity = true;
        else {
            cout << "Error: unrecognized command line option, required --affinity." << endl;
            exit(1);
        }
    }
}
