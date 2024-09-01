#include <iostream>
#include <fstream>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <iomanip>  // For formatting output

#include "config.hpp"

using namespace std;

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

void generateTrace(const string& filename, int numTasks) {
    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Error: Could not open file for writing." << endl;
        return;
    }

    srand(static_cast<unsigned>(time(0))); // Seed for random number generation

    for (int task = 1; task <= numTasks; ++task) {
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
        outFile << "T" << dec << task << ": 0x" << hex << setw(4) << setfill('0') << address 
                << ":" << formatSize(memSize) << endl;
    }

    outFile.close();
    cout << "Trace file generated: " << filename << endl;
}

int main() {
    string filename = "tracefile.txt";
    int numTasks; // Number of tasks to generate traces for
    cout<<"Enter the number of Tasks you want to generate the trace for: ";
    cin>>numTasks;
    generateTrace(filename, numTasks);
    return 0;
}
