#include <getopt.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <deque>


class IO {
public:
    int arrival_time;
    int access_track;

    explicit IO(int arr_time, int access) {
        arrival_time = arr_time;
        access_track = access;
    }
};

class Scheduler {
public:
    virtual IO *get_next_io() = 0;
};

class FCFSScheduler: public Scheduler {
public:
    IO *get_next_io() override {
        return new IO(0,0);
    }
};

/**
 * Global Variables
 */
bool VERBOSE = false;
bool SHOW_Q = false;
bool SHOW_F = false;
Scheduler *SCHEDULER = nullptr;


Scheduler *getScheduler(char *optarg) {
    return nullptr;
}

/**
 * Read command-line arguments and assign values to global variables
 *
 * @param - argc - total argument count
 * @param - argv - array of arguments
 */
void read_arguments(int argc, char **argv) {
    int option;
    while ((option = getopt(argc, argv, "vqfs:")) != -1) {
        switch (option) {
            case 'v':
                VERBOSE = true;
                break;
            case 'q':
                SHOW_Q = true;
                break;
            case 'f':
                SHOW_F = true;
                break;
            case 's': {
                SCHEDULER = getScheduler(optarg);
                break;
            }
            default:
                printf("Usage: ./iosched [-v] inputfile\n");
                exit(1);
        }
    }

    if (argc == optind) {
        printf("Not a valid inputfile <(null)>\n");
        exit(1);
    }

    if (!SCHEDULER) {
        SCHEDULER = new FCFSScheduler();
    }
}

int main(int argc, char **argv) {
    read_arguments(argc, argv);
//    parse_randoms(argv[optind + 1]);
//    load_processes(argv[optind]);
//
//    DISPATCHER = new DES_Layer();
//    DISPATCHER->initialize(PROCESSES);
//    run_simulation();
//
//    print_output();
//
//    garbage_collection();
}