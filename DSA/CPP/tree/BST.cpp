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

// Node class
class Node {
public:
    int data;
    Node* left;
    Node* right;

    Node(int val) {
        data = val;
        left = nullptr;
        right = nullptr;
    }
};

// BST class
class BST {
public:
    Node* root;

    BST() {
        root = nullptr;
    }

    // Insert a new value
    void insert(int val) {
        root = insertHelper(root, val);
    }

    // Helper function for insertion
    Node* insertHelper(Node* node, int val) {
        if (node == nullptr) {
            return new Node(val);
        }
        if (val < node->data) {
            node->left = insertHelper(node->left, val);
        } else {
            node->right = insertHelper(node->right, val);
        }
        return node;
    }

    // Search for a value
    bool search(int val) {
        return searchHelper(root, val);
    }

    // Helper function for searching
    bool searchHelper(Node* node, int val) {
        if (node == nullptr) {
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

    void inOrderHelper(Node* node) {
        if (node != nullptr) {
            inOrderHelper(node->left);
            std::cout << node->data << " ";
            inOrderHelper(node->right);
        }
    }

        // Delete a node
    void deleteNode(int val) {
        root = deleteNodeHelper(root, val);
    }

    // Helper function for deleting a node
    Node* deleteNodeHelper(Node* node, int val) {
        if (node == nullptr) {
            return node;
        }

        // Find the node to be deleted
        if (val < node->data) {
            node->left = deleteNodeHelper(node->left, val);
        } else if (val > node->data) {
            node->right = deleteNodeHelper(node->right, val);
        } else {
            // Node with only one child or no child
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                return temp;
            }

            // Node with two children: Get the inorder successor
            Node* temp = minValueNode(node->right);

            // Copy the inorder successor's content to this node
            node->data = temp->data;

            // Delete the inorder successor
            node->right = deleteNodeHelper(node->right, temp->data);
        }

        return node;
    }

    // Function to find the node with minimum value (used in delete)
    Node* minValueNode(Node* node) {
        Node* current = node;

        // Find the leftmost leaf
        while (current && current->left != nullptr) {
            current = current->left;
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

    //test delete
    std::cout << "In-order Traversal before deletion: ";
    tree.inOrder();

    tree.deleteNode(4); // Delete a node with one child
    tree.deleteNode(3); // Delete a node with two children

    std::cout << "In-order Traversal after deletion: ";
    tree.inOrder();

    return 0;
}
