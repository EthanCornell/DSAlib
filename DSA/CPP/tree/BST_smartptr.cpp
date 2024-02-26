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
#include <memory>

// Node class
class Node {
public:
    int data;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

    Node(int val) : data(val), left(nullptr), right(nullptr) {}
};

// BST class
class BST {
public:
    std::unique_ptr<Node> root;

    BST() : root(nullptr) {}

    // Insert a new value
    void insert(int val) {
        insertHelper(root, val);
    }

    // Helper function for insertion
    void insertHelper(std::unique_ptr<Node>& node, int val) {
        if (!node) {
            node = std::make_unique<Node>(val);
        } else if (val < node->data) {
            insertHelper(node->left, val);
        } else {
            insertHelper(node->right, val);
        }
    }

    // Search for a value
    bool search(int val) {
        return searchHelper(root, val);
    }

    // Helper function for searching
    bool searchHelper(std::unique_ptr<Node>& node, int val) {
        if (!node) {
            return false;
        }
        if (val == node->data) {
            return true;
        } else if (val < node->data) {
            return searchHelper(node->left, val);
        } else {
            return searchHelper(node->right, val);
        }
    }

    // In-order traversal
    void inOrder() {
        inOrderHelper(root);
        std::cout << std::endl;
    }

    void inOrderHelper(std::unique_ptr<Node>& node) {
        if (node) {
            inOrderHelper(node->left);
            std::cout << node->data << " ";
            inOrderHelper(node->right);
        }
    }

    // Delete a node
    void deleteNode(int val) {
        deleteNodeHelper(root, val);
    }

    // Helper function for deleting a node
    void deleteNodeHelper(std::unique_ptr<Node>& node, int val) {
        if (!node) {
            return;
        }

        if (val < node->data) {
            deleteNodeHelper(node->left, val);
        } else if (val > node->data) {
            deleteNodeHelper(node->right, val);
        } else {
            if (!node->left) {
                node = std::move(node->right);
            } else if (!node->right) {
                node = std::move(node->left);
            } else {
                Node* temp = minValueNode(node->right);
                node->data = temp->data;
                deleteNodeHelper(node->right, temp->data);
            }
        }
    }

    // Function to find the node with minimum value (used in delete)
    Node* minValueNode(std::unique_ptr<Node>& node) {
        Node* current = node.get();
        while (current && current->left) {
            current = current->left.get();
        }
        return current;
    }
};

// Main function
int main() {
    BST tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);

    std::cout << "In-order Traversal: ";
    tree.inOrder();

    std::cout << "Search for 4: " << (tree.search(4) ? "Found" : "Not Found") << std::endl;
    std::cout << "Search for 6: " << (tree.search(6) ? "Found" : "Not Found") << std::endl;

    std::cout << "In-order Traversal before deletion: ";
    tree.inOrder();

    tree.deleteNode(4);
    tree.deleteNode(3);

    std::cout << "In-order Traversal after deletion: ";
    tree.inOrder();

    return 0;
}
