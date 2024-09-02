#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>

// Configurable Parameters
#define PAGE_SIZE_BITS 12
#define VIRTUAL_MEMORY_SIZE_BITS 32
#define PHYSICAL_MEMORY_SIZE_BITS 32
#define PAGE_METHOD 1 // 0 for map, 1 for single, 2 for multi-level

// Derived Constants
#define PAGE_SIZE (1ULL << PAGE_SIZE_BITS) // Size of one page in bytes
#define VIRTUAL_MEMORY_SIZE (1ULL << VIRTUAL_MEMORY_SIZE_BITS) // Virtual memory size in bytes
#define PHYSICAL_MEMORY_SIZE (1ULL << PHYSICAL_MEMORY_SIZE_BITS) // Physical memory size in bytes
#define NUMBER_OF_LEVELS 2 // Number of levels in the page table

// Page Table Configurations for Multi-level Page Tables
#define NUMBER_OF_PAGES (VIRTUAL_MEMORY_SIZE / PAGE_SIZE) // Total number of pages
#define NUMBER_OF_FRAMES (PHYSICAL_MEMORY_SIZE / PAGE_SIZE) // Total number of frames
#define PAGE_TABLE_SIZE (NUMBER_OF_PAGES * sizeof(uint32_t)) // Size of the page table in bytes
#define OFFSET_BITS PAGE_SIZE_BITS // Number of bits for the offset within a page
#define BITS_VIRTUAL_PAGE_ADDRESS (VIRTUAL_MEMORY_SIZE_BITS - OFFSET_BITS) // Bits for virtual page address
#define BITS_PHYSICAL_PAGE_ADDRESS (PHYSICAL_MEMORY_SIZE_BITS - OFFSET_BITS) // Bits for physical page address

// Base Addresses for Different Memory Sections
#define TEXT_SECTION_START 0x00001000 // Starting address for the text section
#define DATA_SECTION_START 0x00200000 // Starting address for the data section
#define STACK_SECTION_START 0x40000000 // Starting address for the stack section
#define SHARED_LIB_SECTION_START 0x80000000 // Starting address for the shared library section
#define HEAP_SECTION_START 0xC0000000 // Starting address for the heap section


#endif // CONFIG_HPP
