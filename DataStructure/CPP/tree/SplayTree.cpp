/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2024, Ethan Huang
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>

using namespace std;

// Node structure
struct Node {
    int key;
    Node* left;
    Node* right;

    Node(int val) : key(val), left(nullptr), right(nullptr) {}
};

class SplayTree {
private:
    Node* root;

    // Right rotate
    Node* rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        y->right = x;
        return y;
    }

    // Left rotate
    Node* leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }

    // Splay operation to bring the key to the root
    Node* splay(Node* root, int key) {
        if (!root || root->key == key) return root;

        // Key is in the left subtree
        if (key < root->key) {
            if (!root->left) return root;

            // Zig-Zig (left-left rotation)
            if (key < root->left->key) {
                root->left->left = splay(root->left->left, key);
                root = rightRotate(root);
            }
            // Zig-Zag (left-right rotation)
            else if (key > root->left->key) {
                root->left->right = splay(root->left->right, key);
                if (root->left->right)
                    root->left = leftRotate(root->left);
            }

            return (root->left == nullptr) ? root : rightRotate(root);
        }
        // Key is in the right subtree
        else {
            if (!root->right) return root;

            // Zig-Zig (right-right rotation)
            if (key > root->right->key) {
                root->right->right = splay(root->right->right, key);
                root = leftRotate(root);
            }
            // Zig-Zag (right-left rotation)
            else if (key < root->right->key) {
                root->right->left = splay(root->right->left, key);
                if (root->right->left)
                    root->right = rightRotate(root->right);
            }

            return (root->right == nullptr) ? root : leftRotate(root);
        }
    }

    // Insert node
    Node* insert(Node* root, int key) {
        if (!root) return new Node(key);

        root = splay(root, key);

        if (root->key == key) return root;

        Node* newNode = new Node(key);
        if (key < root->key) {
            newNode->right = root;
            newNode->left = root->left;
            root->left = nullptr;
        } else {
            newNode->left = root;
            newNode->right = root->right;
            root->right = nullptr;
        }

        return newNode;
    }

    // Search node
    Node* search(Node* root, int key) {
        return splay(root, key);
    }

    // Delete node
    Node* deleteNode(Node* root, int key) {
        if (!root) return nullptr;

        root = splay(root, key);

        if (root->key != key) return root;

        Node* temp;
        if (!root->left) {
            temp = root->right;
        } else {
            temp = splay(root->left, key);
            temp->right = root->right;
        }

        delete root;
        return temp;
    }

    // In-order traversal
    void inorder(Node* root) {
        if (!root) return;
        inorder(root->left);
        cout << root->key << " ";
        inorder(root->right);
    }

    // Print tree structure
    void printTree(Node* root, int depth) {
        if (root == nullptr) return;

        // First print the right subtree
        printTree(root->right, depth + 1);

        // Print the current node
        for (int i = 0; i < depth; i++) {
            cout << "    ";  // Indent to represent depth
        }
        cout << root->key << endl;

        // Then print the left subtree
        printTree(root->left, depth + 1);
    }

public:
    SplayTree() : root(nullptr) {}

    void insert(int key) {
        root = insert(root, key);
    }

    void search(int key) {
        root = search(root, key);
    }

    void deleteNode(int key) {
        root = deleteNode(root, key);
    }

    void inorder() {
        inorder(root);
        cout << endl;
    }

    void printTree() {
        printTree(root, 0);  // Initial depth is 0
    }
};

int main() {
    SplayTree tree;

    // Insert nodes
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(40);
    tree.insert(50);
    tree.insert(25);
    
    cout << "In-order traversal result: ";
    tree.inorder();  // In-order traversal result
    cout << "Print tree structure:" << endl;
    tree.printTree();  // Print tree structure

    // Search node
    cout << "Search 30 and print tree structure:" << endl;
    tree.search(30);
    tree.inorder(); 
    tree.printTree();

    // Delete node
    cout << "Delete 40: ";
    tree.deleteNode(40);
    tree.inorder();
    tree.printTree();

    return 0;
}
