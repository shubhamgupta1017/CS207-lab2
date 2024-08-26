#include "io.hpp"
#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;

// Helper function to convert memory size to a human-readable format
string convertMemorySize(unsigned long long int size) {
    if (size < 1024) {
        return to_string(size) + " B";
    } else if (size < 1024 * 1024) {
        return to_string(size / 1024) + " KB";
    } else {
        return to_string(size / (1024 * 1024)) + " MB";
    }
}

void split_input(const string& input, unsigned long long int& task_num, unsigned long long int& logical_address, unsigned long long int& mem) {
    string formatted_input = input.substr(1);
    unsigned long long int index = 0;
    string temp;

    // Extract task number (before ':')
    while (formatted_input[index] != ':') {
        temp += formatted_input[index];
        index++;
    }
    task_num = stoull(temp); // Convert task number to integer

    // Skip past ':' and find 'x'
    while (formatted_input[index] != 'x') {
        index++;
    }
    index++; // Move past 'x'

    // Extract logical address (in hexadecimal, before ':')
    temp.clear();
    while (formatted_input[index] != ':') {
        temp += formatted_input[index];
        index++;
    }
    logical_address = stoull(temp, nullptr, 16); // Convert logical address to integer

    // Skip past ':' and extract memory size (before 'K' or 'M')
    index++; // Move past ':'
    temp.clear();
    while (formatted_input[index] != 'K' && formatted_input[index] != 'M') {
        temp += formatted_input[index];
        index++;
    }
    mem = stoull(temp); // Convert memory size to integer

    // Convert memory size to bytes based on unit ('K' or 'M')
    if (formatted_input[index] == 'K') {
        mem *= 1024; // Convert kilobytes to bytes
    } else if (formatted_input[index] == 'M') {
        mem *= 1024 * 1024; // Convert megabytes to bytes
    }
}

unsigned long long int do_task_map(unsigned long long int logical_address, unsigned long long int memory_req, PageTableMap& page_table, PhysicalMemory& physical_memory) {
    unsigned long long int num_pages = (memory_req + PAGE_SIZE - 1) / PAGE_SIZE;  // Ceiling division to account for partial pages
    unsigned long long int virtual_frame = logical_address >> OFFSET_BITS;
    unsigned long long int offset = logical_address & ((1 << OFFSET_BITS) - 1);
    for (unsigned long long int i = 0; i < num_pages; i++) {
        if (page_table.check(virtual_frame + i) == -1) {  // Page found in page table
            cout << "page hit\n";
            page_table.hit++;  // PERFORMANCE MODULE - HITS 
            return 0;
        } else {
            page_table.miss++; // PERFORMANCE MODULE - MISS
        }
    }
    cout << "start" << endl;
    for (unsigned long long int i = 0; i < num_pages; i++) {
        unsigned long long int physical_frame = page_table.insert_page(virtual_frame + i, physical_memory);
        if (physical_frame != -1) {
            physical_frame = physical_frame << OFFSET_BITS;
            physical_frame += offset;
            cout << "Physical Address: 0X" << hex << uppercase << physical_frame << " to 0X" << hex << uppercase << physical_frame + PAGE_SIZE << endl; // DOUBT
        }
    }
    cout << "end" << endl;
    return num_pages * PAGE_SIZE;
}

unsigned long long int do_task_single(unsigned long long int logical_address, unsigned long long int memory_req, SinglePageTable& page_table, PhysicalMemory& physical_memory) {
    unsigned long long int num_pages = (memory_req + PAGE_SIZE - 1) / PAGE_SIZE;  // Ceiling division to account for partial pages
    unsigned long long int virtual_frame = logical_address >> OFFSET_BITS;
    unsigned long long int offset = logical_address & ((1 << OFFSET_BITS) - 1);
    for (unsigned long long int i = 0; i < num_pages; i++) {
        if (page_table.check(virtual_frame + i) == -1) {
            cout << "page hit\n";
            page_table.hit++;  // PERFORMANCE MODULE - HITS 
            return 0;
        } else {
            page_table.miss++; // PERFORMANCE MODULE - MISS
        }
    }
    cout << "start" << endl;
    for (unsigned long long int i = 0; i < num_pages; i++) {
        unsigned long long int physical_frame = page_table.insert_page(virtual_frame + i, physical_memory);
        if (physical_frame != -1) {
            physical_frame = physical_frame << OFFSET_BITS;
            physical_frame += offset;
            cout << "Physical Address: 0X" << hex << uppercase << physical_frame << " to 0X" << hex << uppercase << physical_frame + PAGE_SIZE << endl;
        }
    }
    cout << "end" << endl;
    return num_pages * PAGE_SIZE;
}

unsigned long long int do_task_multiple(unsigned long long int logical_address, unsigned long long int memory_req, MultiLevelPageTable& page_table, PhysicalMemory& physical_memory) {
    unsigned long long int num_pages = (memory_req + PAGE_SIZE - 1) / PAGE_SIZE;  // Ceiling division to account for partial pages
    unsigned long long int virtual_frame = logical_address >> OFFSET_BITS;
    unsigned long long int offset = logical_address & ((1 << OFFSET_BITS) - 1);
    for (unsigned long long int i = 0; i < num_pages; i++) {
        if (page_table.check(virtual_frame + i) == -1) {
            cout << "page hit\n";
            page_table.hit++;  // PERFORMANCE MODULE - HIT
            return 0;
        } else {
            page_table.miss++; // PERFORMANCE MODULE - MISS
        }
    }
    cout << "start" << endl;
    for (unsigned long long int i = 0; i < num_pages; i++) {
        unsigned long long int physical_frame = page_table.insert_page(virtual_frame + i, physical_memory);
        if (physical_frame != -1) {
            physical_frame = physical_frame << OFFSET_BITS;
            physical_frame += offset;
            cout << "Physical Address: 0X" << hex << uppercase << physical_frame << " to 0X" << hex << uppercase << physical_frame + PAGE_SIZE << endl;
        }
    }
    cout << "end" << endl;
    return num_pages * PAGE_SIZE;
}

pair<int, int> map(const string& filename, ofstream& outFile) {
    PhysicalMemory physical_memory;
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error: Could not open file " << filename << endl;
        return {-1, -1};
    }

    string input;
    unordered_map<int, PageTableMap> task_table_map;
    unsigned long long int total_alloc = 0;
    while (getline(inputFile, input)) {
        unsigned long long int task_num, logical_address, memory_req;
        split_input(input, task_num, logical_address, memory_req);

        // Create a PageTableMap if it does not exist
        if (task_table_map.find(task_num) == task_table_map.end()) {
            task_table_map[task_num] = PageTableMap();
        }

        unsigned long long int mem_alloc = 0;
        // Process the task
        mem_alloc = do_task_map(logical_address, memory_req, task_table_map[task_num], physical_memory);
        total_alloc += mem_alloc;
        if (mem_alloc == 0) {
            outFile << "Memory already allocated to Task T" << task_num << endl;
        } else {
            outFile << "Physical Memory allocated to Task T" << task_num << ": " << convertMemorySize(mem_alloc) << endl;
        }
    }
    outFile << "--------------HITS & MISSES---------------" << endl;
    for (auto x : task_table_map) {
        unsigned long long int t = x.first;
        PageTableMap pt = x.second;
        unsigned long long int hit = pt.hit;
        unsigned long long int miss = pt.miss;
        outFile << "Task: T" << t << " hits: " << hit << " misses: " << miss << endl;
    }
    return {total_alloc, 0}; // Doubt: why returning 0 for the second value?
}

pair<int, int> single(const string& filename, ofstream& outFile) {
    PhysicalMemory physical_memory;
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error: Could not open file " << filename << endl;
        return {-1, -1};
    }

    string input;
    unordered_map<int, SinglePageTable> task_table_single;
    unsigned long long int tot_alloc = 0, num_page_tables = 0;
    while (getline(inputFile, input)) {
        unsigned long long int task_num, logical_address, memory_req;
        split_input(input, task_num, logical_address, memory_req);

        // Create a SinglePageTable if it does not exist
        if (task_table_single.find(task_num) == task_table_single.end()) {
            task_table_single[task_num] = SinglePageTable();
            num_page_tables++;
        }

        // Process the task
        unsigned long long int mem_alloc = 0;
        mem_alloc = do_task_single(logical_address, memory_req, task_table_single[task_num], physical_memory);
        tot_alloc += mem_alloc;
        if (mem_alloc == 0) {
            outFile << "Memory already allocated to Task T" << task_num << endl;
        } else {
            outFile << "Physical Memory allocated to Task T" << task_num << ": " << convertMemorySize(mem_alloc) << endl;
        }
    }
    outFile << "--------------HITS & MISSES---------------" << endl;
    for (auto x : task_table_single) {
        unsigned long long int t = x.first;
        SinglePageTable pt = x.second;
        unsigned long long int hit = pt.hit;
        unsigned long long int miss = pt.miss;
        outFile << "Task: T" << t << " hits: " << hit << " misses: " << miss << endl;
    }
    return {tot_alloc, num_page_tables};
}

pair<int, int> multi(const string& filename, ofstream& outFile) {
    PhysicalMemory physical_memory;
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Error: Could not open file " << filename << endl;
        return {-1, -1};
    }

    string input;
    unordered_map<int, MultiLevelPageTable> task_table_multi;
    unsigned long long int tot_alloc = 0, num_page_tables = 0;
    while (getline(inputFile, input)) {
        unsigned long long int task_num, logical_address, memory_req;
        split_input(input, task_num, logical_address, memory_req);

        // Create a MultiLevelPageTable if it does not exist
        if (task_table_multi.find(task_num) == task_table_multi.end()) {
            task_table_multi[task_num] = MultiLevelPageTable();
            num_page_tables++;
        }

        // Process the task
        unsigned long long int mem_alloc;
        mem_alloc = do_task_multiple(logical_address, memory_req, task_table_multi[task_num], physical_memory);
        tot_alloc += mem_alloc;
        if (mem_alloc == 0) {
            outFile << "Memory already allocated to Task T" << task_num << endl;
        } else {
            outFile << "Physical Memory allocated to Task T" << task_num << ": " << convertMemorySize(mem_alloc) << endl;
        }
    }
    outFile << "--------------HITS & MISSES---------------" << endl;
    for (auto x : task_table_multi) {
        unsigned long long int t = x.first;
        MultiLevelPageTable pt = x.second;
        unsigned long long int hit = pt.hit;
        unsigned long long int miss = pt.miss;
        outFile << "Task: T" << t << " hits: " << hit << " misses: " << miss << endl;
    }
    return {tot_alloc, num_page_tables};
}
