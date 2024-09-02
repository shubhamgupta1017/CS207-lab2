#include <iostream>
#include <fstream>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <iomanip>  // For formatting output
#include <pthread.h>
#include <vector>

#include "config.hpp"

using namespace std;

struct ThreadData {
    int startTaskId;
    int endTaskId;
    int tracesPerTaskId;  // Number of traces to generate for each task ID
    string outputFilename;
};

string formatMemorySize(int sizeInBytes) {
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

void* generateTrace(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    ofstream outFile(data->outputFilename, ios::app);
    if (!outFile) {
        cerr << "Error: Could not open file for writing." << endl;
        return nullptr;
    }

    srand(static_cast<unsigned>(time(0)) + pthread_self()); // Seed for random number generation

    for (int taskId = data->startTaskId; taskId <= data->endTaskId; ++taskId) {
        for (int i = 0; i < data->tracesPerTaskId; ++i) {
            int section = rand() % 5;  // 0: text, 1: data, 2: stack, 3: shared lib, 4: heap

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

            // Write to file in the specified format
            outFile << "Task" << dec << taskId << ": 0x" << hex << setw(4) << setfill('0') << address 
                    << ":" << formatMemorySize(memSize) << endl;
        }
    }

    outFile.close();
    pthread_exit(nullptr);
}

int main() {
    string filename = "tracefile.txt";
    int numTaskIds;        // Number of task IDs to generate traces for
    int numThreads;        // Number of threads to use
    int tracesPerTaskId;   // Number of traces to generate for each task ID

    cout << "Enter the number of task IDs you want to generate traces for: ";
    cin >> numTaskIds;

    cout << "Enter the number of threads to use: ";
    cin >> numThreads;

    cout << "Enter the number of traces to generate for each task ID: ";
    cin >> tracesPerTaskId;

    // Ensure the file is cleared before starting
    ofstream clearFile(filename, ios::out);
    clearFile.close();

    vector<pthread_t> threads(numThreads);
    vector<ThreadData> threadData(numThreads);

    int tasksPerThread = numTaskIds / numThreads;
    int remainingTasks = numTaskIds % numThreads;

    int currentTaskId = 1;
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].startTaskId = currentTaskId;
        threadData[i].endTaskId = currentTaskId + tasksPerThread - 1;
        threadData[i].tracesPerTaskId = tracesPerTaskId;
        if (remainingTasks > 0) {
            threadData[i].endTaskId++;
            remainingTasks--;
        }
        threadData[i].outputFilename = filename;

        pthread_create(&threads[i], nullptr, generateTrace, &threadData[i]);
        currentTaskId = threadData[i].endTaskId + 1;
    }

    // Wait for all threads to complete
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    cout << "Trace file generated: " << filename << endl;
    return 0;
}
