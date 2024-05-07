#include <getopt.h>
#include <fstream>
#include <vector>
#include <cstring>

using namespace std;

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

    virtual ~Scheduler() = default;
};

class FCFSScheduler : public Scheduler {
public:
    IO *get_next_io() override {
        return new IO(0, 0);
    }
};

/**
 * Global Variables
 */
bool VERBOSE = false;           // Show verbose execution trace
bool SHOW_Q = false;            // Show verbose IO Queue Information
bool SHOW_F = false;            // Show Additional information for FLOOK
Scheduler *SCHEDULER = nullptr; // Scheduler instance being used in simulation
vector<IO *> IO_REQUESTS;       // initialize a list of IO Requests

/**
 * Get the appropriate scheduler based on the arguments
 * @param - args - string that needs to parsed to fetch the scheduler type
 *
 * @returns - the correct Scheduler based on the arguments
 */
Scheduler *get_scheduler(char *args) {
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
                SCHEDULER = get_scheduler(optarg);
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

/**
 * Parse the io information from the input file
 * @param - filename - input file
 */
void load_io_requests(char *filename) {
    fstream input_file;

    input_file.open(filename, ios::in);

    if (!input_file.is_open()) {
        printf("Not a valid inputfile <%s>\n", filename);
        exit(1);
    }

    string line;
    while (getline(input_file, line)) {
        if (line[0] == '#')
            continue;
        int arrival_time, access_track;
        char *buffer = new char[line.length() + 1];
        strcpy(buffer, line.c_str());
        sscanf(buffer, "%d %d", &arrival_time, &access_track);
        auto *io = new IO(arrival_time, access_track);
        IO_REQUESTS.push_back(io);
        delete[] buffer;
    }
}

/**
 * Debug function to pretty print the input tokens
 */
[[maybe_unused]] void print_input() {
    int count = 0;
    for (IO *io: IO_REQUESTS) {
        printf("%5d: %5d %5d\n", count, io->arrival_time, io->access_track);
    }
}

/**
 * Deallocate memory used in the program
 */
void garbage_collection() {
    delete SCHEDULER;

    for (IO *io: IO_REQUESTS)
        delete io;
}

int main(int argc, char **argv) {
    read_arguments(argc, argv);
    load_io_requests(argv[optind]);
    print_input();
    //
    //    DISPATCHER = new DES_Layer();
    //    DISPATCHER->initialize(PROCESSES);
    //    run_simulation();
    //
    //    print_output();
    //
    garbage_collection();
}