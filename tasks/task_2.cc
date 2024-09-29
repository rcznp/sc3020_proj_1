#include "../bptree.h"
#include "../disk.h"
#include <iostream>

int main() {
    // Initialize the Disk with a file path (if needed for any operations in the tree)
    // Disk disk("test_disk.db");

    // Create a new B+ Tree instance
    BPTree newBPTree;

    // Deserialize the B+ Tree from the saved file
    newBPTree.deserialize("../bptree_structure.dat");

    // Report the statistics
    std::cout << "Parameter n of the B+ Tree (order): " << BPTREE_ORDER << std::endl;
    std::cout << "Number of nodes in the B+ Tree: " << newBPTree.getNumberOfNodes() << std::endl;
    std::cout << "Number of levels in the B+ Tree: " << newBPTree.getHeight() << std::endl;

    // Print the keys of the root node
    newBPTree.printRootKeys();

    return 0;
}
//g++ task_2.cc ../bptree.cc ../disk.cc -o task_2 -std=c++11



