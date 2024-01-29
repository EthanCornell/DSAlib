/*
 * Copyright (c) Cornell University.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: I-Hsuan (Ethan) Huang
 * Email: ih246@cornell.edu
 */
// Hash Array Mapped Trie (HAMT)
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

// Abstract Node class
class Node {
public:
    virtual ~Node() {}
};

// LeafNode stores the key-value pair
class LeafNode : public Node {
public:
    std::string key;
    std::string value;

    LeafNode(const std::string& key, const std::string& value)
        : key(key), value(value) {}
};

// InternalNode stores child nodes
class InternalNode : public Node {
public:
    std::unordered_map<size_t, std::shared_ptr<Node>> children;
};


// Node Structure: Define a base Node class with subclasses InternalNode and
// LeafNode. Hash Function: Implement a hash function to generate hash codes for
// keys.
class HAMT {
private:
    std::shared_ptr<InternalNode> root;

    // Hash function
    size_t hashFunction(const std::string& key) {
        std::hash<std::string> hasher;
        return hasher(key);
    }

    // Recursive insert helper
    void insertHelper(std::shared_ptr<InternalNode>& node, size_t hash,
                      int level, const std::string& key,
                      const std::string& value) {
        size_t index =
            (hash >> (level * 5)) & 31; // Extract 5 bits from the hash
        if (node->children.find(index) == node->children.end()) {
            // Insert new LeafNode
            node->children[index] = std::make_shared<LeafNode>(key, value);
        } else {
            // Handle collision
            auto& child = node->children[index];
            if (auto leaf = std::dynamic_pointer_cast<LeafNode>(child)) {
                if (leaf->key == key) {
                    leaf->value = value; // Update existing key
                } else {
                    // Create new internal node
                    auto newInternal = std::make_shared<InternalNode>();
                    size_t leafHash = hashFunction(leaf->key);
                    insertHelper(newInternal, leafHash, level + 1, leaf->key,
                                 leaf->value);
                    insertHelper(newInternal, hash, level + 1, key, value);
                    node->children[index] = newInternal;
                }
            } else if (auto internal =
                           std::dynamic_pointer_cast<InternalNode>(child)) {
                insertHelper(internal, hash, level + 1, key, value);
            }
        }
    }

    // Recursive remove helper
    bool removeHelper(std::shared_ptr<InternalNode>& node, size_t hash,
                      int level, const std::string& key) {
        size_t index =
            (hash >> (level * 5)) & 31; // Extract 5 bits from the hash
        if (node->children.find(index) == node->children.end()) {
            return false; // Key not found
        }

        auto& child = node->children[index];
        if (auto leaf = std::dynamic_pointer_cast<LeafNode>(child)) {
            if (leaf->key == key) {
                // Key found, remove leaf
                node->children.erase(index);
                return true;
            }
            return false; // Key not found
        } else if (auto internal =
                       std::dynamic_pointer_cast<InternalNode>(child)) {
            if (removeHelper(internal, hash, level + 1, key)) {
                // If the internal node is now empty, remove it
                if (internal->children.empty()) {
                    node->children.erase(index);
                }
                return true;
            }
        }
        return false;
    }

public:
    HAMT() : root(std::make_shared<InternalNode>()) {}

    // Insertion:
    // Hash the key.
    // Traverse the trie using the hash code.
    // Insert a new leaf node or update an existing one.
    void insert(const std::string& key, const std::string& value) {
        size_t hash = hashFunction(key);
        insertHelper(root, hash, 0, key, value);
    }

    // Lookup:
    // Hash the key.
    // Traverse the trie using the hash code.
    // Return the value if the key is found.
    std::string lookup(const std::string& key) {
        std::shared_ptr<InternalNode> node = root;
        size_t hash = hashFunction(key);
        int level = 0;

        while (node != nullptr) {
            size_t index = (hash >> (level * 5)) & 31;
            if (node->children.find(index) == node->children.end()) {
                return "Key not found"; // Key not found
            }

            auto child = node->children[index];
            if (auto leaf = std::dynamic_pointer_cast<LeafNode>(child)) {
                if (leaf->key == key) {
                    return leaf->value; // Key found
                }
                return "Key not found"; // Key not found
            } else if (auto internal =
                           std::dynamic_pointer_cast<InternalNode>(child)) {
                node = internal;
                level++;
            }
        }

        return "Key not found"; // Key not found
    }

    // Deletion:
    // Hash the key.
    // Traverse the trie, removing the node if found.
    void remove(const std::string& key) {
        size_t hash = hashFunction(key);
        removeHelper(root, hash, 0, key);
    }
};


// Time Complexity:
// Insertion, Lookup, and Deletion: O(log N) on average, where N is the number
// of elements. This is due to the hash function and the trie traversal. Space
// Complexity: O(N), where N is the number of elements stored. Each node in the
// trie occupies a constant amount of space.

int main() {
    HAMT trie;
    trie.insert("key1", "value1");
    trie.insert("key2", "value2");

    std::cout << "Lookup key1: " << trie.lookup("key1") << std::endl;
    std::cout << "Lookup key2: " << trie.lookup("key2") << std::endl;

    trie.remove("key1"); // Remove function not implemented
    std::cout << "Lookup key1 after deletion: " << trie.lookup("key1")
              << std::endl;

    return 0;
}
