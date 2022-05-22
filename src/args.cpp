#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

// Checks the command line arguments and returns the K given as parameter.
void check_arguments(int argn, char *argc[], string* filename, float* detection_percentage, size_t* workers, bool* thread_affinity) {
    // Check command line arguments
    if(!(2 <= argn && argn <= 5)) {
        cout << "Usage: " << argc[0] << " <filename> [detection percentage] [number of workers] [--affinity]" << endl;
        exit(1);
    }

    // Read the filename parameter from the command line
    *filename = argc[1];

    // Check that the file exists
    ifstream file(*filename);
    if(!file.good()) {
        cout << "Error: file " << *filename << " cannot be opened." << endl;
        exit(1);
    }

    // Read the optional detection_percentage parameter from the command line
    if(argn >= 3) {
        *detection_percentage = atof(argc[2]);
        if(!(0.0 <= *detection_percentage && *detection_percentage <= 1.0)) {
            cout << "Error: detection_percentage must be a valid float percentage between 0 and 1." << endl;
            exit(1);
        }
    }

    // Read the workers parameter from the command line
    if(argn >= 4) {
        *workers = atoi(argc[2]);
        if(*workers <= 0) {
            cout << "Error: the number of workers must be a valid number greater than zero." << endl;
            exit(1);
        }
    }

    // Read the affinity option from the command line
    if(argn >= 4) {
        if(strcmp(argc[3], "--affinity") == 0)
            *thread_affinity = true;
        else {
            cout << "Error: unrecognized command line option, required --affinity." << endl;
            exit(1);
        }
    }
}
