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

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

class Node {
public:
    int data;
    vector<Node*> children;

    Node(int data) : data(data) {}
};

class M_aryTree {
private:
    Node* root;
    int m;

public:
    M_aryTree(int m) : m(m), root(NULL) {}

    // Function to insert nodes in the tree
    void insert(int parentData, int data) {
        if (!root) {
            root = new Node(data);
            return;
        }
        Node* parentNode = search(root, parentData);
        if (parentNode && parentNode->children.size() < m) {
            parentNode->children.push_back(new Node(data));
        }
    }

    // Utility function for searching a node
    Node* search(Node* node, int data) {
        if (!node) return nullptr;
        if (node->data == data) return node;

        for (auto& child : node->children) {
            Node* result = search(child, data);
            if (result) return result;
        }
        return nullptr;
    }
    
    // Function to delete a node
    void deleteNode(int data) {
        if (!root) return;
        if (root->data == data) {
            deleteSubTree(root);
            root = NULL;
            return;
        }
        deleteNodeHelper(root, data);
    }

    // Helper function for deleteNode to recursively delete a subtree
    void deleteSubTree(Node* node) {
        for (auto child : node->children) {
            deleteSubTree(child);
        }
        delete node;
    }

    // Helper function to find and delete a node
    void deleteNodeHelper(Node* node, int data) {
        for (auto& child : node->children) {
            if (child && child->data == data) {
                deleteSubTree(child);
                child = nullptr;
                node->children.erase(remove(node->children.begin(), node->children.end(), nullptr), node->children.end());
                return;
            } else {
                deleteNodeHelper(child, data);
            }
        }
    }

    // Function to perform DFS traversal
    void traverseDFS(Node* node) {
        if (!node) return;
        cout << node->data << " ";
        for (auto child : node->children) {
            traverseDFS(child);
        }
    }

    // Function to print the tree
    void printTree() {
        traverseDFS(root);
        cout << endl;
    }
};

// Main function with test cases
int main() {
    M_aryTree tree(3); // Creating an m-ary tree with m = 3
    // Insert nodes
    tree.insert(0, 1); // First node, acts as root
    tree.insert(1, 2);
    tree.insert(1, 3);
    tree.insert(1, 4); // Root now has 3 children
    tree.insert(2, 5); // Adding a child to one of the root's children
    tree.insert(3, 6);

    cout << "Tree before deletion:" << endl;
    tree.printTree();

    // Delete a node and print again
    tree.deleteNode(3);
    cout << "Tree after deleting node 3:" << endl;
    tree.printTree();

    return 0;
}