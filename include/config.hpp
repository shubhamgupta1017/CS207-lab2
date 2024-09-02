#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>
//changeable parameters
#define PAGE_SIZE_BITS 12
#define VIRTUAL_MEMORY_SIZE_BITS 32
#define PHYSICAL_MEMORY_SIZE_BITS 32
#define PAGE_METHOD 2//0 for map and 1 for single and 2 for multi


#define PAGE_SIZE (unsigned long long int)(1ULL<<PAGE_SIZE_BITS)           //size entry  size of one pag3
#define VIRTUAL_MEMORY_SIZE (1ULL << VIRTUAL_MEMORY_SIZE_BITS) // 1MB virtual memory
#define PHYSICAL_MEMORY_SIZE (1ULL << PHYSICAL_MEMORY_SIZE_BITS) // 1MB physical memory
#define NUMBER_OF_LEVELS 2    // Number of levels in the page table

// Page table configurations for multi-level page tables
#define NUMBER_OF_PAGES VIRTUAL_MEMORY_SIZE / (unsigned long long int)PAGE_SIZE
#define NUMBER_OF_FRAMES PHYSICAL_MEMORY_SIZE / (unsigned long long int)PAGE_SIZE
#define PAGE_TABLE_SIZE NUMBER_OF_PAGES * sizeof(uint32_t)
#define OFFSET_BITS PAGE_SIZE_BITS
#define BITS_VIRTUAL_PAGE_ADDRESS VIRTUAL_MEMORY_SIZE_BITS-OFFSET_BITS
#define BITS_PHYSICAL_PAGE_ADDRESS PHYSICAL_MEMORY_SIZE_BITS-OFFSET_BITS

//BASE ADDRESSES
#define TEXT_SECTION_START 0x1000           // Example starting address for text section
#define DATA_SECTION_START 0x200000         // Example starting address for data section
#define STACK_SECTION_START 0x40000000      // Example starting address for stack section
#define SHARED_LIB_SECTION_START 0x80000000 // Example starting address for shared library section
#define HEAP_SECTION_START 0xC0000000       // Example starting address for heap section


#endif 
