// record_block.h
#ifndef RECORD_BLOCK_H
#define RECORD_BLOCK_H

#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>

// Constants based on script output
const size_t GAME_DATE_EST_SIZE = 11;    // 10 bytes + 1 for null-termination
const size_t TEAM_ID_HOME_SIZE = 4; // 4 bytes (uint32_t)
const size_t PTS_HOME_SIZE = 4;          // 4 bytes (uint32_t)
const size_t FG_PCT_HOME_SIZE = 5;       // 5 bytes
const size_t FT_PCT_HOME_SIZE = 5;       // 5 bytes
const size_t FG3_PCT_HOME_SIZE = 5;      // 5 bytes
const size_t AST_HOME_SIZE = 2;          // 2 bytes (uint16_t)
const size_t REB_HOME_SIZE = 2;          // 2 bytes (uint16_t)
const size_t HOME_TEAM_WINS_SIZE = 1;    // 1 byte (bool)

const size_t BLOCK_SIZE = 4096;          // Standard block size (can be adjusted as needed)

// Record Structure with packed alignment
#pragma pack(push, 1)
struct Record {
    char game_date_est[GAME_DATE_EST_SIZE];    // "GAME_DATE_EST" - Max 10 characters
    uint32_t team_id_home;                     // "TEAM_ID_home" - Numeric ID
    uint32_t pts_home;                         // "PTS_home" - Max 3 digits, stored as uint32_t
    char fg_pct_home[FG_PCT_HOME_SIZE];        // "FG_PCT_home" - Max 5 characters (e.g., "0.456")
    char ft_pct_home[FT_PCT_HOME_SIZE];        // "FT_PCT_home" - Max 5 characters
    char fg3_pct_home[FG3_PCT_HOME_SIZE];      // "FG3_PCT_home" - Max 5 characters
    uint16_t ast_home;                         // "AST_home" - Max 2 digits
    uint16_t reb_home;                         // "REB_home" - Max 2 digits
    bool home_team_wins;                       // "HOME_TEAM_WINS" - 1 byte (true/false)

    // Serialize the Record into a byte buffer
    void serialize(uint8_t* buffer) const;

    // Deserialize the Record from a byte buffer
    void deserialize(const uint8_t* buffer);

    // Function to print a Record (useful for debugging)
    void print() const;
};
#pragma pack(pop)

// Static assertion to ensure Record size is as expected
static_assert(sizeof(Record) == 39, "Record size must be 39 bytes");

// Calculate RECORDS_PER_BLOCK based on the actual size of Record and space for numRecords
// const size_t RECORDS_PER_BLOCK = 104; // Manually set to 104 to prevent overflow


const size_t RECORDS_PER_BLOCK = (BLOCK_SIZE - sizeof(uint32_t)) / sizeof(Record);

// Block Structure with numRecords first
struct Block {
    uint32_t numRecords;                        // Number of records in the block
    Record records[RECORDS_PER_BLOCK];          // Array of records

    // Constructor to initialize numRecords
    Block() : numRecords(0) {}

    // Serialize the Block into a byte buffer
    void serialize(uint8_t* buffer) const;

    // Deserialize the Block from a byte buffer
    void deserialize(const uint8_t* buffer);

    // Function to print all Records in the Block (useful for debugging)
    void print() const;
};

// Function to print block statistics
void printBlockStatistics(size_t totalRecords);

#endif // RECORD_BLOCK_H
