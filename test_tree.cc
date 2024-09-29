#include "bptree.h"
#include "disk.h"
#include <iostream>

int main() {
    // Initialize the Disk with a file path
    Disk disk("bptree_disk.dat");

    // Initialize the B+ Tree
    BPTree bptree;

    // Insert some keys into the B+ Tree
    bptree.insert(10.0, disk.GetNextFreeBlock(), disk);
    bptree.insert(20.0, disk.GetNextFreeBlock(), disk);
    bptree.insert(5.0, disk.GetNextFreeBlock(), disk);
    bptree.insert(15.0, disk.GetNextFreeBlock(), disk);

    // Traverse the B+ Tree to check the structure
    std::cout << "B+ Tree structure after inserts:" << std::endl;
    bptree.traverse(disk);
    std::cout << std::endl;

    // Serialize the B+ Tree to disk
    bptree.serialize("bptree_structure.dat");

    // Clear the B+ Tree and load it again from disk
    BPTree newBPTree;
    newBPTree.deserialize("bptree_structure.dat");

    // Traverse the deserialized B+ Tree
    std::cout << "B+ Tree structure after deserialization:" << std::endl;
    newBPTree.traverse(disk);

    return 0;
}


// g++ -std=c++11 test_tree.cc disk.cc bptree.cc -o test_tree
