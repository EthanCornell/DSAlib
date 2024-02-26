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
#include <unordered_map>
#include <iostream>
#include <string>

using namespace std;

struct TrieNode {
    bool isEndOfWord;
    unordered_map<char, TrieNode*> children;

    TrieNode() : isEndOfWord(false) {}
};


void insert(TrieNode* root, string key) {
    TrieNode* node = root;
    for (char c : key) {
        if (!node->children.count(c)) {
            node->children[c] = new TrieNode();
        }
        node = node->children[c];
    }
    node->isEndOfWord = true;
}

bool search(TrieNode* root, string key) {
    TrieNode* node = root;
    for (char c : key) {
        if (!node->children.count(c)) {
            return false;
        }
        node = node->children[c];
    }
    return node != nullptr && node->isEndOfWord;
}


bool deleteHelper(TrieNode* node, string key, int depth = 0) {
    if (!node) return false;

    // If last character of key is being processed
    if (depth == key.size()) {
        if (node->isEndOfWord) {
            node->isEndOfWord = false;
            return node->children.empty();
        }
        return false;
    }

    char c = key[depth];
    if (node->children.find(c) == node->children.end()) return false;

    bool shouldDeleteCurrentNode = deleteHelper(node->children[c], key, depth + 1);

    if (shouldDeleteCurrentNode) {
        delete node->children[c];
        node->children.erase(c);

        // Return true if current node has no children
        return !node->isEndOfWord && node->children.empty();
    }

    return false;
}

void deleteKey(TrieNode* root, string key) {
    deleteHelper(root, key);
}

int main() {
    TrieNode* root = new TrieNode();

    insert(root, "hello");
    cout << search(root, "hello") << endl;  // 1 for true

    deleteKey(root, "hello");
    cout << search(root, "hello") << endl;  // 0 for false

    // Remember to free the allocated memory for Trie
    // Implement a function for this as an exercise

    return 0;
}
