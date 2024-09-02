#include <iostream>
#include <fstream>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <iomanip>  // For formatting output
#include <pthread.h> // For pthreads

#include "config.hpp" // Ensure this file defines the required constants

using namespace std;

struct TaskParams {
    int taskID;
    ofstream* outFile;
    pthread_mutex_t* mutex;
    int numTraces; // Number of traces to generate per task
};

string formatSize(int sizeInBytes) {
    const int KB = 1024;  // 1 KB = 1024 bytes
    const int MB = 1024 * KB;  // 1 MB = 1024 KB

    if (sizeInBytes >= MB) {
        double sizeInMB = static_cast<double>(sizeInBytes) / MB;
        return to_string(static_cast<int>(sizeInMB)) + "MB";
    } else if (sizeInBytes >= KB) {
        double sizeInKB = static_cast<double>(sizeInBytes) / KB;
        return to_string(static_cast<int>(sizeInKB)) + "KB";
    } else {
        return to_string(sizeInBytes) + "B";
    }
}

void* generateTask(void* arg) {
    TaskParams* params = static_cast<TaskParams*>(arg);
    int taskID = params->taskID;
    ofstream* outFile = params->outFile;
    pthread_mutex_t* mutex = params->mutex;
    int numTraces = params->numTraces;

    // Generate the specified number of traces
    for (int i = 0; i < numTraces; ++i) {
        // Randomly select a memory section
        int section = rand() % 5; // 0: text, 1: data, 2: stack, 3: shared lib, 4: heap

        // Determine the base address based on the selected section
        int baseAddress;
        switch (section) {
            case 0: baseAddress = TEXT_SECTION_START; break;
            case 1: baseAddress = DATA_SECTION_START; break;
            case 2: baseAddress = STACK_SECTION_START; break;
            case 3: baseAddress = SHARED_LIB_SECTION_START; break;
            case 4: baseAddress = HEAP_SECTION_START; break;
            default: baseAddress = TEXT_SECTION_START; // Default to text section
        }

        // Generate a random address within the section, aligned to PAGE_SIZE
        int address = baseAddress + (rand() % 0x10000) * PAGE_SIZE; // Adjust range if needed

        // Generate a random memory size between 1KB and 16KB
        int memSize = (rand() % 16 + 1) * PAGE_SIZE;

        // Lock the mutex before writing to the file
        pthread_mutex_lock(mutex);

        // Write the trace information to the file
        *outFile << "T" << dec << taskID << ": 0x" << hex << setw(4) << setfill('0') << address 
                 << ":" << formatSize(memSize) << endl;

        // Unlock the mutex after writing
        pthread_mutex_unlock(mutex);
    }

    pthread_exit(nullptr); // Terminate the thread
    return nullptr;
}

void generateTrace(const string& filename, int numTasks, int numTracesPerTask) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error: Could not open file for writing." << endl;
        return;
    }

    srand(static_cast<unsigned>(time(0))); // Seed for random number generation

    pthread_t threads[numTasks]; // Array maintaining threads
    TaskParams params[numTasks]; // Parameters corresponding to each thread

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr); // Initialize the mutex

    for (int task = 1; task <= numTasks; ++task) { // Thread creation
        params[task - 1].taskID = task;
        params[task - 1].outFile = &outFile;
        params[task - 1].mutex = &mutex;
        params[task - 1].numTraces = numTracesPerTask;
        pthread_create(&threads[task - 1], nullptr, generateTask, &params[task - 1]);
    }

    for (int task = 1; task <= numTasks; ++task) {  // Waiting for all the threads to finish
        pthread_join(threads[task - 1], nullptr);
    }

    outFile.close();
    pthread_mutex_destroy(&mutex); // Destroy the mutex

    cout << "Trace file generated: " << filename << endl;
}

int main() {
    string filename = "tracefile.txt";
    int numTasks; // Number of task IDs (threads) to generate traces for
    int numTracesPerTask; // Number of traces to generate for each task ID

    cout << "Enter the number of tasks (threads): ";
    cin >> numTasks;
    cout << "Enter the number of traces per task: ";
    cin >> numTracesPerTask;

    if (numTasks > 0 && numTracesPerTask > 0) {
        generateTrace(filename, numTasks, numTracesPerTask); // Generate the trace file
    } else {
        cout << "Number of tasks and number of traces per task must be greater than 0." << endl;
    }

    return 0;
}
