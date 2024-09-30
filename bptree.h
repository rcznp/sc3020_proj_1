#ifndef BPTREE_H
#define BPTREE_H

#include "disk.h"
#include "record_block.h"
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

const int BPTREE_ORDER = 5;  // B+ Tree order

// B+ Tree Node structure
class BPTreeNode : public std::enable_shared_from_this<BPTreeNode> {
public:
    bool isLeaf;
    std::vector<float> keys;
    std::vector<std::shared_ptr<BPTreeNode>> children;
    std::vector<std::vector<uint32_t>> records; // Record pointers (leaf)
    std::shared_ptr<BPTreeNode> nextLeaf;

    BPTreeNode(bool isLeaf);

    void insertNonFull(float key, uint32_t recordPointer, Disk& disk);
    void splitChild(int i, std::shared_ptr<BPTreeNode> y, Disk& disk);
    void traverse(Disk& disk);
    std::shared_ptr<BPTreeNode> search(float key, Disk& disk);

    // Serialization and Deserialization functions
    void serialize(std::ofstream& outFile) const;
    void deserialize(std::ifstream &inFile, std::vector<std::shared_ptr<BPTreeNode>> &leafNodes);
    int countNodes() const;      // Method to count total number of nodes
    int computeHeight() const;   // Method to compute the height of the tree
};

class BPTree {
public:
    std::shared_ptr<BPTreeNode> root;
    BPTree();

    void insert(float key, uint32_t recordPointer, Disk& disk);
    void traverse(Disk& disk);

    // Serialization and Deserialization
    void serialize(const std::string& filePath) const;
    void deserialize(const std::string& filePath);

    // New methods for node counting, height, and root key printing
    int getNumberOfNodes() const;  // Get total number of nodes in the B+ Tree
    int getHeight() const;         // Get height of the B+ Tree
    void printRootKeys() const;    // Print the keys in the root node
    std::vector<uint32_t> rangeQuery(float startKey, float endKey, Disk& disk);
    std::shared_ptr<BPTreeNode> findLeafNode(float key) const;
};

#endif // BPTREE_H
