#include <getopt.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <deque>

using namespace std;

class IO {
public:
    int arrival_time;
    int access_track;
    unsigned long start_time;
    unsigned long end_time;

    explicit IO(int arr_time, int access) : arrival_time(arr_time), access_track(access), start_time(0), end_time(0) {}
};

vector<IO *> IO_REQUESTS;       // initialize a list of IO Requests
IO *ACTIVE_IO = nullptr;        // currently active IO

class Scheduler {
protected:
    deque<IO *> io_queue;
public:
    virtual IO *get_next() = 0;

    virtual void add_to_queue(IO *io) {
        io_queue.push_back(io);
    };

    virtual ~Scheduler() = default;

    virtual bool has_pending() {
        return !io_queue.empty();
    }
};

class FCFSScheduler : public Scheduler {
public:
    IO *get_next() override {
        IO *io = io_queue.front();
        io_queue.pop_front();
        return io;
    }
};

class SSTFScheduler : public Scheduler {
public:
    IO *get_next() override {
        return new IO(0, 0);
    }
};

class LOOKScheduler : public Scheduler {
public:
    IO *get_next() override {
        return new IO(0, 0);
    }
};

class CLOOKScheduler : public Scheduler {
public:
    IO *get_next() override {
        return new IO(0, 0);
    }
};

class FLOOKScheduler : public Scheduler {
public:
    IO *get_next() override {
        return new IO(0, 0);
    }
};

/**
 * Global Variables
 */
bool VERBOSE = false;           // Show verbose execution trace
bool SHOW_Q = false;            // Show verbose IO Queue Information
bool SHOW_F = false;            // Show Additional information for FLOOK
bool FORWARD = false;           // Indicates whether the disk track is mooving forward
Scheduler *SCHEDULER = nullptr; // Scheduler instance being used in simulation
unsigned long CURR_TIME = 0;    // Current time
unsigned long CURR_TRACK = 0;   // Current disk track


/**
 * Get the appropriate scheduler based on the arguments
 * @param - args - string that needs to parsed to fetch the scheduler type
 *
 * @returns - the correct Scheduler based on the arguments
 */
Scheduler *get_scheduler(const char *args) {
    switch (args[0]) {
        case 'N':
            return new FCFSScheduler();
        case 'S':
            return new SSTFScheduler();
        case 'L':
            return new LOOKScheduler();
        case 'C':
            return new CLOOKScheduler();
        case 'F':
            return new FLOOKScheduler();
        default:
            printf("Unknown Scheduler spec: -s {NSLCF}\n");
            exit(1);
    }
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
 * Start simulation
 */
void run_simulation() {
    int curr_request_index = 0;
    while (true) {
        auto next_io = IO_REQUESTS[curr_request_index];
        if (next_io->arrival_time == CURR_TIME) {
            SCHEDULER->add_to_queue(next_io);
            curr_request_index++;
        }
        if (ACTIVE_IO) {
            if (ACTIVE_IO->access_track == CURR_TRACK) {
                ACTIVE_IO->end_time = CURR_TIME;
                ACTIVE_IO = nullptr;
            } else {
                CURR_TRACK += FORWARD ? 1 : -1;
            }
        } else if (SCHEDULER->has_pending()) {
            ACTIVE_IO = SCHEDULER->get_next();
            ACTIVE_IO->start_time = CURR_TIME;
            if (ACTIVE_IO->access_track != CURR_TRACK) {
                FORWARD = CURR_TRACK < ACTIVE_IO->access_track;
            }
        }

        if (!ACTIVE_IO &&
            !SCHEDULER->has_pending() &&
            curr_request_index == IO_REQUESTS.size())
            break;
        CURR_TIME++;
    }
}

/**
 * Debug function to pretty print the input tokens
 */
[[maybe_unused]] void print_input() {
    int count = 0;
    for (IO *io: IO_REQUESTS) {
        printf("%5d: %5d %5d\n", count, io->arrival_time, io->access_track);
        count++;
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
    run_simulation();
//    print_output();
    garbage_collection();
}