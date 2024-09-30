// record_block.cc

#include "record_block.h"
#include <iostream>

// Function to print block statistics
void printBlockStatistics(size_t totalRecords) {
    size_t recordSize = sizeof(Record);
    size_t recordsPerBlock = RECORDS_PER_BLOCK;
    size_t totalBlocks = (totalRecords + recordsPerBlock - 1) / recordsPerBlock; // Calculate the number of blocks required

    std::cout << "Statistics:\n";
    std::cout << "Size of a record: " << recordSize << " bytes\n";
    std::cout << "Number of records: " << totalRecords << "\n";
    std::cout << "Number of records stored in a block: " << recordsPerBlock << "\n";
    std::cout << "Number of blocks for storing the data: " << totalBlocks << "\n";
}

// Serialize the Record into a byte buffer
void Record::serialize(uint8_t* buffer) const {
    size_t offset = 0;

    // Serialize game_date_est
    std::memcpy(buffer + offset, game_date_est, GAME_DATE_EST_SIZE);
    offset += GAME_DATE_EST_SIZE;

    // Serialize team_id_home
    std::memcpy(buffer + offset, &team_id_home, sizeof(team_id_home));
    offset += sizeof(team_id_home);

    // Serialize pts_home
    std::memcpy(buffer + offset, &pts_home, sizeof(pts_home));
    offset += sizeof(pts_home);

    // Serialize fg_pct_home
    std::memcpy(buffer + offset, fg_pct_home, FG_PCT_HOME_SIZE);
    offset += FG_PCT_HOME_SIZE;

    // Serialize ft_pct_home
    std::memcpy(buffer + offset, ft_pct_home, FT_PCT_HOME_SIZE);
    offset += FT_PCT_HOME_SIZE;

    // Serialize fg3_pct_home
    std::memcpy(buffer + offset, fg3_pct_home, FG3_PCT_HOME_SIZE);
    offset += FG3_PCT_HOME_SIZE;

    // Serialize ast_home
    std::memcpy(buffer + offset, &ast_home, sizeof(ast_home));
    offset += sizeof(ast_home);

    // Serialize reb_home
    std::memcpy(buffer + offset, &reb_home, sizeof(reb_home));
    offset += sizeof(reb_home);

    // Serialize home_team_wins
    std::memcpy(buffer + offset, &home_team_wins, sizeof(home_team_wins));
    offset += sizeof(home_team_wins);
}

// Deserialize the Record from a byte buffer
void Record::deserialize(const uint8_t* buffer) {
    size_t offset = 0;

    // Deserialize game_date_est
    std::memcpy(game_date_est, buffer + offset, GAME_DATE_EST_SIZE);
    game_date_est[GAME_DATE_EST_SIZE - 1] = '\0'; // Ensure null-termination
    offset += GAME_DATE_EST_SIZE;

    // Deserialize team_id_home
    std::memcpy(&team_id_home, buffer + offset, sizeof(team_id_home));
    offset += sizeof(team_id_home);

    // Deserialize pts_home
    std::memcpy(&pts_home, buffer + offset, sizeof(pts_home));
    offset += sizeof(pts_home);

    // Deserialize fg_pct_home
    std::memcpy(fg_pct_home, buffer + offset, FG_PCT_HOME_SIZE);
    fg_pct_home[FG_PCT_HOME_SIZE - 1] = '\0'; // Ensure null-termination
    offset += FG_PCT_HOME_SIZE;

    // Deserialize ft_pct_home
    std::memcpy(ft_pct_home, buffer + offset, FT_PCT_HOME_SIZE);
    ft_pct_home[FT_PCT_HOME_SIZE - 1] = '\0'; // Ensure null-termination
    offset += FT_PCT_HOME_SIZE;

    // Deserialize fg3_pct_home
    std::memcpy(fg3_pct_home, buffer + offset, FG3_PCT_HOME_SIZE);
    fg3_pct_home[FG3_PCT_HOME_SIZE - 1] = '\0'; // Ensure null-termination
    offset += FG3_PCT_HOME_SIZE;

    // Deserialize ast_home
    std::memcpy(&ast_home, buffer + offset, sizeof(ast_home));
    offset += sizeof(ast_home);

    // Deserialize reb_home
    std::memcpy(&reb_home, buffer + offset, sizeof(reb_home));
    offset += sizeof(reb_home);

    // Deserialize home_team_wins
    std::memcpy(&home_team_wins, buffer + offset, sizeof(home_team_wins));
    offset += sizeof(home_team_wins);
}

// Function to print a Record (useful for debugging)
void Record::print() const {
    std::cout << "GAME_DATE_EST: " << game_date_est << ", "
              << "TEAM_ID_home: " << team_id_home << ", "
              << "PTS_home: " << pts_home << ", "
              << "FG_PCT_home: " << fg_pct_home << ", "
              << "FT_PCT_home: " << ft_pct_home << ", "
              << "FG3_PCT_home: " << fg3_pct_home << ", "
              << "AST_home: " << ast_home << ", "
              << "REB_home: " << reb_home << ", "
              << "HOME_TEAM_WINS: " << (home_team_wins ? "True" : "False") << std::endl;
}

// Serialize the Block into a byte buffer
void Block::serialize(uint8_t* buffer) const {
    size_t offset = 0;

    // Serialize numRecords
    std::memcpy(buffer + offset, &numRecords, sizeof(numRecords));
    offset += sizeof(numRecords);

    // Serialize each Record
    for (uint32_t i = 0; i < numRecords; ++i) {
        records[i].serialize(buffer + offset);
        offset += sizeof(Record); // Move offset by actual record size
    }

    // Fill the remaining buffer with zeros if necessary
    if (offset < BLOCK_SIZE) {
        std::memset(buffer + offset, 0, BLOCK_SIZE - offset);
    }
}

// Deserialize the Block from a byte buffer
void Block::deserialize(const uint8_t* buffer) {
    size_t offset = 0;

    // Deserialize numRecords
    std::memcpy(&numRecords, buffer + offset, sizeof(numRecords));
    offset += sizeof(numRecords);

    // Safety check to prevent reading more records than possible
    if (numRecords > RECORDS_PER_BLOCK) {
        std::cerr << "Error: numRecords (" << numRecords << ") exceeds RECORDS_PER_BLOCK ("
                  << RECORDS_PER_BLOCK << "). Truncating to RECORDS_PER_BLOCK.\n";
        numRecords = RECORDS_PER_BLOCK;
    }

    // Deserialize each Record
    for (uint32_t i = 0; i < numRecords; ++i) {
        records[i].deserialize(buffer + offset);
        offset += sizeof(Record); // Move offset by actual record size
    }

    // Optionally, handle any remaining data if necessary
}

// Function to print all Records in the Block (useful for debugging)
void Block::print() const {
    std::cout << "Block with " << numRecords << " records:\n";
    for (uint32_t i = 0; i < numRecords; ++i) {
        std::cout << "Record " << (i + 1) << ": ";
        records[i].print();
    }
}