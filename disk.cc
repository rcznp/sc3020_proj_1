// disk.cc
#include "disk.h"
#include <iostream>
#include <cstring>

// Constructor
Disk::Disk(const std::string& path) 
    : filePath(path), 
      currentBlock(0), 
      totalRecordsInCurrentBlock(0), 
      totalBlocks(1), 
      recordsPerBlock(RECORDS_PER_BLOCK) { // Initialize from record_block.h
    std::cout << "Attempting to open disk file: " << filePath << std::endl;

    // Open the disk file in read/write mode, create if it doesn't exist
    diskFile.open(filePath, std::ios::in | std::ios::out | std::ios::binary);

    if (!diskFile.is_open()) {
        std::cerr << "Error: Could not open disk file: " << filePath << std::endl;
        std::cerr << "Attempting to create the file." << std::endl;

        // Create the file if it doesn't exist
        diskFile.clear();
        diskFile.open(filePath, std::ios::out | std::ios::binary);
        if (!diskFile.is_open()) {
            std::cerr << "Error: Could not create disk file: " << filePath << std::endl;
            return;
        }
        std::cout << "File created successfully." << std::endl;

        diskFile.close();
        // Reopen in read/write mode
        diskFile.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!diskFile.is_open()) {
            std::cerr << "Error: Could not reopen disk file: " << filePath << std::endl;
            return;
        }
    } else {
        std::cout << "File opened successfully." << std::endl;

        // Determine total blocks by file size
        diskFile.seekg(0, std::ios::end);
        std::streampos fileSize = diskFile.tellg();
        totalBlocks = static_cast<uint32_t>(fileSize) / BLOCK_SIZE;
        diskFile.seekg(0, std::ios::beg);
        std::cout << "Total blocks on disk: " << totalBlocks << std::endl;
    }
}

// Destructor
Disk::~Disk() {
    if (diskFile.is_open()) {
        std::cout << "Closing the disk file: " << filePath << std::endl;
        diskFile.close();
    }
}

// Write data to a block
bool Disk::WriteBlock(uint32_t blockNumber, const uint8_t* buffer) {
    std::cout << "Writing to block: " << blockNumber << std::endl;

    // Expand the disk file if necessary
    if (blockNumber >= totalBlocks) {
        std::cout << "Expanding disk for block number: " << blockNumber << std::endl;
        diskFile.seekp(0, std::ios::end);
        uint32_t newBlocks = blockNumber - totalBlocks + 1;
        char zero = 0;
        for (uint32_t i = 0; i < newBlocks; ++i) {
            for (uint32_t j = 0; j < BLOCK_SIZE; ++j) {
                diskFile.write(&zero, 1); // Write zeros to expand the file
            }
        }
        totalBlocks += newBlocks;
        std::cout << "Disk expanded, new total blocks: " << totalBlocks << std::endl;
    }

    // Seek to the block and write the data
    diskFile.seekp(blockNumber * BLOCK_SIZE, std::ios::beg);
    diskFile.write(reinterpret_cast<const char*>(buffer), BLOCK_SIZE);
    if (!diskFile) {
        std::cerr << "Error: Failed to write to block " << blockNumber << std::endl;
        return false;
    }

    diskFile.flush();
    std::cout << "Write operation to block " << blockNumber << " successful." << std::endl;
    return true;
}

// Read data from a block
bool Disk::ReadBlock(uint32_t blockNumber, uint8_t* buffer) {
    std::cout << "Reading from block: " << blockNumber << std::endl;

    if (blockNumber >= totalBlocks) {
        std::cerr << "Error: Block " << blockNumber << " does not exist." << std::endl;
        return false;
    }

    diskFile.seekg(blockNumber * BLOCK_SIZE, std::ios::beg);
    diskFile.read(reinterpret_cast<char*>(buffer), BLOCK_SIZE);
    if (!diskFile) {
        std::cerr << "Error: Failed to read from block " << blockNumber << std::endl;
        return false;
    }
    std::cout << "Read operation from block " << blockNumber << " successful." << std::endl;
    return true;
}

// Get the next free block (dynamic)
uint32_t Disk::GetNextFreeBlock() {
    if (totalRecordsInCurrentBlock >= recordsPerBlock) {
        // Move to the next block when the current block is full
        currentBlock++;
        totalRecordsInCurrentBlock = 0;  // Reset the record count for the new block
        std::cout << "Switching to new block: " << currentBlock << std::endl;
    }
    return currentBlock;
}

// Increment the record count for the current block
void Disk::IncrementRecordCount() {
    totalRecordsInCurrentBlock++;
    std::cout << "Records in current block: " << totalRecordsInCurrentBlock << "/" << recordsPerBlock << std::endl;
}
