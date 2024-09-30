#include "bptree.h"
#include <fstream>

// Constructor for BPTreeNode
BPTreeNode::BPTreeNode(bool isLeaf) : isLeaf(isLeaf) {}

int BPTreeNode::countNodes() const {
    int count = 1; // Count this node
    if (!isLeaf) {
        for (const auto& child : children) {
            count += child->countNodes();
        }
    }
    return count;
}

// Compute the height of the tree
int BPTreeNode::computeHeight() const {
    if (isLeaf) {
        return 1;
    } else {
        // Since it's a balanced tree, all children have the same height
        return 1 + children[0]->computeHeight();
    }
}

void BPTreeNode::insertNonFull(float key, uint32_t recordPointer, Disk& disk) {
    int i = keys.size() - 1;

    if (isLeaf) {
        // Find the position to insert the key
        while (i >= 0 && keys[i] > key) {
            i--;
        }
        i++;

        if (i < keys.size() && keys[i] == key) {
            // Key exists, append the record pointer
            records[i].push_back(recordPointer);
        } else {
            // Insert new key and record pointer
            keys.insert(keys.begin() + i, key);
            records.insert(records.begin() + i, std::vector<uint32_t>{recordPointer});
        }
    } else {
        // Find the child to insert into
        while (i >= 0 && keys[i] > key) i--;
        i++;

        if (children[i]->keys.size() == BPTREE_ORDER - 1) {
            splitChild(i, children[i], disk);
            if (key > keys[i]) i++;
        }
        children[i]->insertNonFull(key, recordPointer, disk);
    }
}

void BPTreeNode::splitChild(int i, std::shared_ptr<BPTreeNode> y, Disk& disk) {
    int mid = y->keys.size() / 2;
    std::shared_ptr<BPTreeNode> z = std::make_shared<BPTreeNode>(y->isLeaf);

    if (y->isLeaf) {
        // Split keys and records
        z->keys.assign(y->keys.begin() + mid, y->keys.end());
        z->records.assign(y->records.begin() + mid, y->records.end());

        y->keys.resize(mid);
        y->records.resize(mid);

        // Update nextLeaf pointers
        z->nextLeaf = y->nextLeaf;
        y->nextLeaf = z;

        // Insert new child
        children.insert(children.begin() + i + 1, z);

        // Promote key to parent
        keys.insert(keys.begin() + i, z->keys[0]);
    } else {
        // Internal node splitting
        float midKey = y->keys[mid];

        z->keys.assign(y->keys.begin() + mid + 1, y->keys.end());
        z->children.assign(y->children.begin() + mid + 1, y->children.end());

        y->keys.resize(mid);
        y->children.resize(mid + 1);

        children.insert(children.begin() + i + 1, z);
        keys.insert(keys.begin() + i, midKey);
    }
}

// Traverse the B+ Tree
void BPTreeNode::traverse(Disk& disk) {
    for (int i = 0; i < keys.size(); i++) {
        if (!isLeaf) {
            children[i]->traverse(disk);
        }
        std::cout << " " << keys[i];
    }
    if (!isLeaf) {
        children[keys.size()]->traverse(disk);
    }
}

// Search in B+ Tree
std::shared_ptr<BPTreeNode> BPTreeNode::search(float key, Disk& disk) {
    int i = 0;
    while (i < keys.size() && key > keys[i]) i++;
    if (i < keys.size() && keys[i] == key) return shared_from_this();
    if (isLeaf) return nullptr;
    return children[i]->search(key, disk);
}

// Serialization of BPTreeNode with file pointer checks
void BPTreeNode::serialize(std::ofstream& outFile) const {
    std::cout << "[DEBUG] Serializing node at file pointer: " << outFile.tellp() << "\n";

    outFile.write(reinterpret_cast<const char*>(&isLeaf), sizeof(isLeaf));
    uint32_t numKeys = keys.size();
    outFile.write(reinterpret_cast<const char*>(&numKeys), sizeof(numKeys));

    // Serialize keys
    for (float key : keys) {
        outFile.write(reinterpret_cast<const char*>(&key), sizeof(key));
    }

    if (isLeaf) {
        // Serialize records
        for (const auto& recordList : records) {
            uint32_t numRecords = recordList.size();
            outFile.write(reinterpret_cast<const char*>(&numRecords), sizeof(numRecords));
            for (uint32_t record : recordList) {
                outFile.write(reinterpret_cast<const char*>(&record), sizeof(record));
            }
        }
    } else {
        // Serialize children
        for (const auto& child : children) {
            child->serialize(outFile);
        }
    }

    std::cout << "[DEBUG] Node serialization complete at file pointer: " << outFile.tellp() << "\n";
}

void BPTreeNode::deserialize(std::ifstream& inFile, std::vector<std::shared_ptr<BPTreeNode>>& leafNodes) {
    std::cout << "[DEBUG] Deserializing node at file pointer: " << inFile.tellg() << "\n";

    // Read if node is a leaf
    inFile.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));
    if (!inFile) {
        std::cerr << "[ERROR] Failed to read isLeaf flag.\n";
        return;
    }

    // Read number of keys
    uint32_t numKeys;
    inFile.read(reinterpret_cast<char*>(&numKeys), sizeof(numKeys));
    if (!inFile || numKeys > BPTREE_ORDER || numKeys == 0) {
        std::cerr << "[ERROR] Invalid numKeys: " << numKeys << " at file pointer: " << inFile.tellg() << "\n";
        return;
    }

    // Deserialize keys
    keys.resize(numKeys);
    for (float& key : keys) {
        inFile.read(reinterpret_cast<char*>(&key), sizeof(key));
    }

    if (isLeaf) {
        // Deserialize records
        records.resize(numKeys);
        for (auto& recordList : records) {
            uint32_t numRecords;
            inFile.read(reinterpret_cast<char*>(&numRecords), sizeof(numRecords));
            recordList.resize(numRecords);
            for (uint32_t& record : recordList) {
                inFile.read(reinterpret_cast<char*>(&record), sizeof(record));
            }
        }
        // Collect leaf nodes
        leafNodes.push_back(shared_from_this());
    } else {
        // Deserialize children
        children.resize(numKeys + 1);
        for (auto& child : children) {
            child = std::make_shared<BPTreeNode>(false);
            child->deserialize(inFile, leafNodes);
        }
    }

    std::cout << "[DEBUG] Node deserialization complete at file pointer: " << inFile.tellg() << "\n";
}

// BPTree Constructor
BPTree::BPTree() {
    root = std::make_shared<BPTreeNode>(true);
}

// Get the total number of nodes in the B+ Tree
int BPTree::getNumberOfNodes() const {
    if (root != nullptr) {
        return root->countNodes();
    }
    return 0;
}

// Get the height (number of levels) of the B+ Tree
int BPTree::getHeight() const {
    if (root != nullptr) {
        return root->computeHeight();
    }
    return 0;
}

// Print the keys of the root node
void BPTree::printRootKeys() const {
    if (root != nullptr) {
        std::cout << "Keys in root node: ";
        for (float key : root->keys) {
            std::cout << key << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "The B+ Tree is empty." << std::endl;
    }
}

// Insert into the B+ Tree
void BPTree::insert(float key, uint32_t recordPointer, Disk& disk) {
    if (root->keys.size() == BPTREE_ORDER - 1) {
        std::shared_ptr<BPTreeNode> s = std::make_shared<BPTreeNode>(false);
        s->children.push_back(root);
        s->splitChild(0, root, disk);
        int i = 0;
        if (s->keys[0] < key) i++;
        s->children[i]->insertNonFull(key, recordPointer, disk);
        root = s;
    } else {
        root->insertNonFull(key, recordPointer, disk);
    }
}

// Traverse the B+ Tree
void BPTree::traverse(Disk& disk) {
    if (root != nullptr) root->traverse(disk);
}

void BPTree::serialize(const std::string& filePath) const {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing the B+ Tree" << std::endl;
        return;
    }

    if (root != nullptr) {
        root->serialize(outFile);
    }

    outFile.flush();  // Ensure data is flushed to disk before closing
    std::cout << "[DEBUG] Flushed the output file.\n";

    outFile.close();  // Properly close the file to commit the changes to the filesystem
    if (!outFile) {
        std::cerr << "[ERROR] There was an issue closing the file properly.\n";
    } else {
        std::cout << "[DEBUG] B+ Tree serialization complete at file pointer: " << outFile.tellp() << "\n";
    }
}

// Deserialize the B+ Tree
void BPTree::deserialize(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error: Could not open file for reading the B+ Tree" << std::endl;
        return;
    }
    std::cout << "[DEBUG] Deserializing B+ Tree at file pointer: " << inFile.tellg() << "\n";

    // Create a vector to hold leaf nodes
    std::vector<std::shared_ptr<BPTreeNode>> leafNodes;

    // Deserialize the root node directly
    root = std::make_shared<BPTreeNode>(true);
    root->deserialize(inFile, leafNodes);

    // Link the leaf nodes
    for (size_t i = 0; i + 1 < leafNodes.size(); ++i) {
        leafNodes[i]->nextLeaf = leafNodes[i + 1];
    }
    if (!leafNodes.empty()) {
        leafNodes.back()->nextLeaf = nullptr;
    }

    inFile.close();
    std::cout << "[DEBUG] B+ Tree deserialization complete at file pointer: " << inFile.tellg() << "\n";
}

std::shared_ptr<BPTreeNode> BPTree::findLeafNode(float key) const {
    std::shared_ptr<BPTreeNode> currentNode = root;
    while (!currentNode->isLeaf) {
        int i = 0;
        while (i < currentNode->keys.size() && key >= currentNode->keys[i]) {
            i++;
        }
        currentNode = currentNode->children[i];
    }
    return currentNode;
}

std::vector<uint32_t> BPTree::rangeQuery(float startKey, float endKey, Disk& disk) {
    std::vector<uint32_t> result;

    // Find the leaf node where the startKey would be
    std::shared_ptr<BPTreeNode> currentNode = findLeafNode(startKey);
    if (!currentNode) {
        return result; // Empty result if the tree is empty
    }

    while (currentNode) {
        for (size_t i = 0; i < currentNode->keys.size(); ++i) {
            float key = currentNode->keys[i];
            if (key >= startKey && key <= endKey) {
                // Add all record pointers for this key to the result
                result.insert(result.end(), currentNode->records[i].begin(), currentNode->records[i].end());
            } else if (key > endKey) {
                // We've passed the endKey, can stop the search
                return result;
            }
        }
        // Move to the next leaf node
        currentNode = currentNode->nextLeaf;
    }

    return result;
}
