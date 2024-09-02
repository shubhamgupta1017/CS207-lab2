#ifndef IO_HPP
#define IO_HPP

#include <string>
#include <fstream>
#include <utility>
#include "memory_manager.hpp"  // Adjust the include based on your file structure
#include "task_module.hpp"
#include "config.hpp"

// Function to split the input string into components
void split_input(const std::string& input, unsigned long long int& task_num, unsigned long long int& logical_address, unsigned long long int& mem);

// Functions to perform tasks based on different page table types
unsigned long long int do_task_map(unsigned long long int logical_address, unsigned long long int memory_req, PageTableMap& page_table, PhysicalMemory& physical_memory);
unsigned long long int do_task_single(unsigned long long int logical_address, unsigned long long int memory_req, SinglePageTable& page_table, PhysicalMemory& physical_memory);
unsigned long long int do_task_multiple(unsigned long long int logical_address, unsigned long long int memory_req, MultiLevelPageTable& page_table, PhysicalMemory& physical_memory);

// Functions to process tasks from a file
std::pair<int, int> map(const std::string& filename, std::ofstream& outFile);
std::pair<int, int> single(const std::string& filename, std::ofstream& outFile);
std::pair<int, int> multi(const std::string& filename, std::ofstream& outFile);

// Function to convert memory size to a human-readable format
std::string convertMemorySize(unsigned long long int memoryInBytes);

#endif // IO_HPP
