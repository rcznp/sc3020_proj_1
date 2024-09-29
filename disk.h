// disk.h
#ifndef DISK_H
#define DISK_H

#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <cstring>
#include "record_block.h"  // Include the record and block definitions

class Disk {
private:
    std::string filePath;
    std::fstream diskFile;
    uint32_t currentBlock;                // The block currently being written to
    uint32_t totalRecordsInCurrentBlock;  // Number of records in the current block
    uint32_t totalBlocks;                 // Total number of blocks allocated
    const uint32_t recordsPerBlock;       // Number of records per block (from record_block.h)

public:
    Disk(const std::string& path);
    ~Disk();

    // Write and read methods
    bool WriteBlock(uint32_t blockNumber, const uint8_t* buffer);
    bool ReadBlock(uint32_t blockNumber, uint8_t* buffer);

    // Dynamic method to get next free block
    uint32_t GetNextFreeBlock();

    // Utility method to increment the record count in the current block
    void IncrementRecordCount();

    uint32_t GetTotalBlocks() const { return totalBlocks; }

    // Check if the disk is open
    bool isOpen() const { return diskFile.is_open(); }
};

#endif // DISK_H
