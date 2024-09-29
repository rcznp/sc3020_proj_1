#include "disk.h"
#include <map>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include "record_block.h"  // Use the existing Record and Block structs
#include "bptree.h"
std::map<std::string, std::vector<std::pair<int, int> > > fgPctIndex;
void findRecordsByFgPct(const std::string& fgPct, Disk& disk) {
    // Check if the fgPct exists in the index
    if (fgPctIndex.find(fgPct) != fgPctIndex.end()) {
        for (const std::pair<int, int>& blockRecordPair : fgPctIndex[fgPct]) {
            int blockNum = blockRecordPair.first;
            int recordNum = blockRecordPair.second;
            
            // Read the block from disk
            uint8_t readBuffer[BLOCK_SIZE];
            std::memset(readBuffer, 0, BLOCK_SIZE);

            if (disk.ReadBlock(blockNum, readBuffer)) {
                Block block;
                block.deserialize(readBuffer);

                // Print the specific record from the block
                std::cout << "Record found in Block " << blockNum << ", Record " << recordNum + 1 << ":\n";
                block.records[recordNum].print();
            } else {
                std::cerr << "Error: Failed to read block " << blockNum << " from disk." << std::endl;
            }
        }
    } else {
        std::cout << "No records found with FG_PCT_home = " << fgPct << std::endl;
    }
}
void visualizeFgPctIndex() {
    std::cout << "FG_PCT_home Index Visualization:" << std::endl;
    for (const auto& entry : fgPctIndex) {
        const std::string& fgPct = entry.first;
        const std::vector<std::pair<int, int>>& records = entry.second;

        std::cout << "FG_PCT_home = " << fgPct << " maps to the following block/record pairs:" << std::endl;
        for (const std::pair<int, int>& blockRecordPair : records) {
            std::cout << "  Block " << blockRecordPair.first << ", Record " << blockRecordPair.second + 1 << std::endl;
        }
    }
}
int main() {
    // Initialize the disk with a test file path
    Disk disk("test_disk.db");
    BPTree bptree;

    // Open the games.txt file
    std::ifstream inputFile("games.txt");
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open games.txt for reading." << std::endl;
        return 1;
    }

    // Skip the header
    std::string header;
    std::getline(inputFile, header);

    std::string line;
    Block blockToWrite;  // Block to accumulate records

    size_t recordCount = 0;
    const size_t maxRecords = 200000000;  // purposely put big so read everything

    // Read each line from the file and populate the records
    while (std::getline(inputFile, line) && recordCount < maxRecords) {
        std::istringstream ss(line);
        Record recordToWrite;

        // Read fields from the line and populate the record
        ss >> std::setw(GAME_DATE_EST_SIZE) >> recordToWrite.game_date_est;
        ss >> recordToWrite.team_id_home;
        ss >> recordToWrite.pts_home;
        ss >> std::setw(FG_PCT_HOME_SIZE) >> recordToWrite.fg_pct_home;
        ss >> recordToWrite.ast_home;
        ss >> recordToWrite.reb_home;
        ss >> recordToWrite.home_team_wins;

        // Add the record to the block
        blockToWrite.records[blockToWrite.numRecords++] = recordToWrite;

        // Add FG_PCT_home to the index with blockIndex and record position
        fgPctIndex[recordToWrite.fg_pct_home].emplace_back(disk.GetNextFreeBlock(), blockToWrite.numRecords - 1);

        // Increment the record count in the block
        disk.IncrementRecordCount();

        // If the block is full, serialize and write it to the disk
        if (blockToWrite.numRecords == RECORDS_PER_BLOCK) {
            uint8_t writeBuffer[BLOCK_SIZE];
            std::memset(writeBuffer, 0, BLOCK_SIZE);
            blockToWrite.serialize(writeBuffer);  // Serialize the block

            if (disk.WriteBlock(disk.GetNextFreeBlock(), writeBuffer)) {
                std::cout << "Block written successfully with " << RECORDS_PER_BLOCK << " records." << std::endl;
                blockToWrite.numRecords = 0;  // Reset the block for new records
            } else {
                std::cerr << "Error: Failed to write block to disk." << std::endl;
                return 1;
            }
        }

        // Increment the record count
        recordCount++;
    }

    // If there are any remaining records in the block, write them to disk
    if (blockToWrite.numRecords > 0) {
        uint8_t writeBuffer[BLOCK_SIZE];
        std::memset(writeBuffer, 0, BLOCK_SIZE);
        blockToWrite.serialize(writeBuffer);  // Serialize the block

        if (disk.WriteBlock(disk.GetNextFreeBlock(), writeBuffer)) {
            std::cout << "Block written successfully with " << blockToWrite.numRecords << " remaining records." << std::endl;
        } else {
            std::cerr << "Error: Failed to write block to disk." << std::endl;
            return 1;
        }
    }

    inputFile.close();
    // Insert FG_PCT_home into the B+ Tree
    for (const auto& entry : fgPctIndex) {
        const std::string& fgPct = entry.first;
        const std::vector<std::pair<int, int>>& records = entry.second;
        
        // We insert the FG_PCT_home as the key and the first block number as the value
        if (!records.empty()) {
            float fgPctFloat = std::stof(fgPct);  // Convert FG_PCT_home to float for insertion
            int blockNumber = records[0].first;   // Use the first block number
            bptree.insert(fgPctFloat, blockNumber, disk);  // Insert into B+ Tree
        }
    }
    

    // // Visualize and search the FG_PCT index
    // visualizeFgPctIndex();
    // std::string searchFgPct;
    // std::cout << "Enter FG_PCT_home to search (e.g., '0.500'): ";
    // std::cin >> searchFgPct;

    // // Search for records by FG_PCT_home using the index
    // findRecordsByFgPct(searchFgPct, disk);
    // Traverse the B+ Tree to check the structure
    std::cout << "B+ Tree structure after inserting FG_PCT_home:" << std::endl;
    bptree.traverse(disk);
    std::cout << std::endl;

    bptree.serialize("bptree_structure.dat");


    return 0;
}

// g++ -std=c++11 test_disk.cc disk.cc record_block.cc bptree.cc -o test_disk