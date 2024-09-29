#include "bptree.h"
#include "disk.h"
#include <iostream>

int main() {
    // Initialize the Disk with a file path (if needed for any operations in the tree)
    Disk disk("test_disk.db");

    // Create a new B+ Tree instance
    BPTree newBPTree;

    // Deserialize the B+ Tree from the saved file
    newBPTree.deserialize("bptree_structure.dat");

    // Traverse the B+ Tree to check the structure
    std::cout << "B+ Tree structure after deserialization:" << std::endl;
    newBPTree.traverse(disk);

    return 0;
}

// Compile with g++ -std=c++11 test_load_tree.cc disk.cc bptree.cc -o test_load_tree
