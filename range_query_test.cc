#include "bptree.h"
#include "disk.h"
#include "record_block.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

int main() {
    // Initialize the Disk
    Disk disk("test_disk.db");

    // Create and deserialize the B+ Tree
    BPTree newBPTree;
    newBPTree.deserialize("bptree_structure.dat");

    // Perform a range query from 0.5 to 0.8 inclusively
    float startKey = 0.6f;
    float endKey = 0.8f;

    std::vector<uint32_t> recordPointers = newBPTree.rangeQuery(startKey, endKey, disk);

    std::cout << "Records with FG_PCT_home in range [" << startKey << ", " << endKey << "]:" << std::endl;

    // Read and print the records from disk
    for (uint32_t recordPointer : recordPointers) {
        uint32_t blockNum = recordPointer; // Assuming recordPointer is the block number

        uint8_t readBuffer[BLOCK_SIZE];
        std::memset(readBuffer, 0, BLOCK_SIZE);

        if (disk.ReadBlock(blockNum, readBuffer)) {
            Block block;
            block.deserialize(readBuffer);

            // Iterate through records in the block
            for (uint32_t i = 0; i < block.numRecords; ++i) {
                float fgPct = std::stof(block.records[i].fg_pct_home);
                if (fgPct >= startKey && fgPct <= endKey) {
                    block.records[i].print();
                }
            }
        } else {
            std::cerr << "Error reading block " << blockNum << " from disk." << std::endl;
        }
    }

    return 0;
}
// Compile with g++ -std=c++11 range_query_test.cc disk.cc bptree.cc record_block.cc -o range_query_test
