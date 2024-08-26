#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include "config.hpp" // Include config.h to get NUMBER_OF_FRAMES
#include <cstdio> // For printf

class PhysicalMemory {
public:
    PhysicalMemory();
    unsigned long long int allocate_page();

private:
    int* frames;
    unsigned long long int frames_used;
};

#endif 
