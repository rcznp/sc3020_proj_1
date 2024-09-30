#include "../record_block.h"

int main() {
    size_t totalRecords = 26652; // later some function to get all lines
    printBlockStatistics(totalRecords);
    return 0;
}
//g++ task_1.cc ../record_block.cc -o task_1