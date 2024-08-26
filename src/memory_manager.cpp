#include "memory_manager.hpp"
#include "config.hpp"

PhysicalMemory::PhysicalMemory() 
    : frames(new int[NUMBER_OF_FRAMES]()), frames_used(0) {
    // Initialize all frames to 0
    for (unsigned long long int i = 0; i < NUMBER_OF_FRAMES; i++) {
        frames[i] = 0;
    }
}

unsigned long long int PhysicalMemory::allocate_page() {
    for (unsigned long long int i = 0; i < NUMBER_OF_FRAMES; i++) {
        if (frames[i] == 0) {
            frames[i] = 1;
            frames_used++;
            return i;
        }
    }
    printf("Physical memory exceeded\n");
    return -1; 
}
