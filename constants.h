#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

// Disk settings
const uint32_t BLOCK_SIZE = 4096;       // 4KB blocks
const uint32_t DISK_SIZE = 10485760;    // 10MB disk size
const uint32_t NUM_BLOCKS = DISK_SIZE / BLOCK_SIZE;
const uint32_t FIRST_DATA_BLOCK = 1;    // Block 0 reserved for metadata or superblock

// Record settings
const int RECORD_SIZE = 62;             // Size of a single record
const int TEAM_NAME_SIZE = 50;          // Maximum characters for team names

// Calculate the number of records that fit into a block
const int RECORDS_PER_BLOCK = (BLOCK_SIZE - sizeof(uint32_t)) / RECORD_SIZE;

#endif // CONSTANTS_H
