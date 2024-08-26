#include "task_module.hpp"
#include "memory_manager.hpp"
#include "config.hpp"
#include <iostream>

using namespace std;

// Level 2 Page Table Implementation

Level2PageTable::Level2PageTable(unsigned long long int number_of_pages)
    : page_table(number_of_pages, 0) {}

unsigned long long int Level2PageTable::insert_page(unsigned long long int virtual_page_address, PhysicalMemory& physical_memory) {
    unsigned long long int physical_frame_address = physical_memory.allocate_page();
    if (physical_frame_address == static_cast<unsigned long long int>(-1)) {
        return -1;
    }
    page_table[virtual_page_address] = (physical_frame_address << OFFSET_BITS) + 1;
    return physical_frame_address;
}

bool Level2PageTable::is_page_valid(unsigned long long int virtual_page_address) const {
    return (page_table[virtual_page_address] & 1) != 0;
}

unsigned long long int Level2PageTable::get_physical_frame(unsigned long long int virtual_page_address) const {
    if (is_page_valid(virtual_page_address)) {
        return page_table[virtual_page_address] >> OFFSET_BITS;
    }
    return static_cast<unsigned long long int>(-1);
}

void Level2PageTable::print_page_table() const {
    for (size_t i = 0; i < page_table.size(); ++i) {
        cout << "Virtual Page " << i << " -> " << page_table[i] << endl;
    }
}

// Multi-Level Page Table Implementation

MultiLevelPageTable::MultiLevelPageTable() {
    bits_per_level[0] = (VIRTUAL_MEMORY_SIZE_BITS + 1) / 2;
    bitmask[0] = (1ULL << bits_per_level[0]) - 1;
    bits_per_level[1] = VIRTUAL_MEMORY_SIZE_BITS - bits_per_level[0];
    bitmask[1] = (1ULL << bits_per_level[1]) - 1;

    page_table_first.resize(1ULL << bits_per_level[0], nullptr);
}

MultiLevelPageTable::~MultiLevelPageTable() {
    for (auto pt : page_table_first) {
        delete pt;
    }
}

unsigned long long int MultiLevelPageTable::insert_page(unsigned long long int virtual_page_address, PhysicalMemory& physical_memory) {
    extract_bits(virtual_page_address);

    if (page_table_first[address[0]] == nullptr) {
        page_table_first[address[0]] = new Level2PageTable(1ULL << bits_per_level[1]);
    }

    return page_table_first[address[0]]->insert_page(address[1], physical_memory);
}

bool MultiLevelPageTable::check(unsigned long long int virtual_page_address) {
    extract_bits(virtual_page_address);
    if (page_table_first[address[0]] == nullptr) {
        return false;
    }
    return page_table_first[address[0]]->is_page_valid(address[1]);
}

void MultiLevelPageTable::extract_bits(unsigned long long int virtual_page_address) {
    for (unsigned long long int i = 0; i < 2; i++) {
        address[i] = virtual_page_address & bitmask[i];
        virtual_page_address >>= bits_per_level[i];
    }
}

// Single Level Page Table Implementation

SinglePageTable::SinglePageTable() {
    page_table = new int[NUMBER_OF_PAGES];
    for (int i = 0; i < NUMBER_OF_PAGES; i++) {
        page_table[i] = 0;
    }
}

unsigned long long int SinglePageTable::insert_page(unsigned long long int virtual_page_address, PhysicalMemory& physical_memory) {
    if (virtual_page_address >= NUMBER_OF_PAGES) {
        cout << "out of range\n";
        return -1;
    }

    unsigned long long int physical_frame_address = physical_memory.allocate_page();
    if (physical_frame_address == static_cast<unsigned long long int>(-1)) {
        return -1;
    }

    page_table[static_cast<int>(virtual_page_address)] = (physical_frame_address << OFFSET_BITS) + 1;
    return physical_frame_address;
}

bool SinglePageTable::check(unsigned long long int virtual_page_address) const {
    return (page_table[static_cast<int>(virtual_page_address)] & 1) == 0;
}

// Page Table using a Map Implementation

unsigned long long int PageTableMap::insert_page(unsigned long long int virtual_page_address, PhysicalMemory& physical_memory) {
    if (virtual_page_address < 0 || virtual_page_address >= VIRTUAL_MEMORY_SIZE) {
        cout << "Invalid virtual page address\n";
        return -1;
    }

    unsigned long long int physical_frame_address = physical_memory.allocate_page();
    if (physical_frame_address == static_cast<unsigned long long int>(-1)) {
        return -1;
    }

    page_table[virtual_page_address] = physical_frame_address;
    return physical_frame_address;
}

unsigned long long int PageTableMap::check(unsigned long long int virtual_page_address) {
    if (page_table.find(virtual_page_address) != page_table.end()) {
        return -1;
    }
    return 1;
}
