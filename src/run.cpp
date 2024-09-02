#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <ctime>
#include "io.hpp"
#include "config.hpp"

using namespace std;

int main() {
    // Define file names for input and output
    string filename = "tracefile_4KB_4GB_4GB.txt";
    ofstream outFile("performance.txt", ios_base::app);
    
    // Check if the output file opened successfully
    if (!outFile.is_open()) {
        cerr << "Error: Could not open file for writing." << endl;
        return 1; // Return with error code
    }

    // Variables to hold total memory allocated and number of page tables created
    pair<int, int> mem; // first -> total memory allocated to all tasks, second -> total page tables created
    unsigned long long int tot_alloc, num_page_tables;

    // Record start time
    clock_t startTime = clock();
    
    // Perform tasks based on the PAGE_METHOD
    if (PAGE_METHOD == 0) {
        mem = map(filename, outFile);
    } else if (PAGE_METHOD == 1) {
        mem = single(filename, outFile);
    } else if (PAGE_METHOD == 2) {
        mem = multi(filename, outFile);
    } else {
        cerr << "Error: Invalid PAGE_METHOD value." << endl;
        return 1; // Return with error code
    }

    // Extract memory allocation and page table count
    tot_alloc = mem.first;
    num_page_tables = mem.second;

    // Calculate available memory
    unsigned long long int avail = (PHYSICAL_MEMORY_SIZE > tot_alloc) ? (PHYSICAL_MEMORY_SIZE - tot_alloc) : 0;
    string mem_avail = convertMemorySize(avail);

    // Output results
    outFile << "Free Available Memory: " << mem_avail << endl;
    unsigned long long int mem_for_page_table = num_page_tables * PAGE_TABLE_SIZE;
    outFile << "Memory allocated to the page tables: " << mem_for_page_table << endl;

    // Record end time and calculate execution time
    clock_t endTime = clock();
    double execTime = double(endTime - startTime) / CLOCKS_PER_SEC;
    outFile << "Execution Time: " << execTime << " seconds" << endl;

    // Close the output file
    outFile.close();

    return 0; // Return with success code
}
