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
#include <queue>
using namespace std;

enum Color { RED, BLACK };

struct Node {
    int data;
    bool color;
    Node *left, *right, *parent;

    // Constructor
    Node(int data) : data(data), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RedBlackTree {
private:
    Node *root;

protected:
    void rotateLeft(Node *&);
    void rotateRight(Node *&);
    void fixViolation(Node *&);

public:
    RedBlackTree() : root(nullptr) {}

    // Function to insert a new node with given data
    void insert(const int &n) {
        Node *node = new Node(n);
        root = BSTInsert(root, node);
        fixViolation(node);
    }

    void deleteNode(const int &n) {
        // Step 1: Delete the node from the tree
        Node *deletedNode = deleteBST(root, n);

        // Step 2: Fix the tree if the deleted node was black
        if (deletedNode != nullptr) {
            fixDeleteRBTree(deletedNode);
        }
    }


    // Function to do inorder traversal
    void inorder() { inorderHelper(root); }

    // Function to do level order traversal
    void levelOrder() { levelOrderHelper(root); }



private:
    // Utility functions
    Node* BSTInsert(Node*&, Node*&);
    void inorderHelper(Node *&);
    void levelOrderHelper(Node *&);
    void fixInsertRBTree(Node *&);
    void fixDeleteRBTree(Node *&);
    int getColor(Node *&);
    void setColor(Node *&, int);
    Node *minValueNode(Node *&);
    Node *maxValueNode(Node *&);
    Node* insertBST(Node *&, Node *&);
    Node* deleteBST(Node *&, int);
};

// Implementations of the utility functions and balancing logic

void RedBlackTree::rotateLeft(Node *&pt) {
    Node *pt_right = pt->right;
    pt->right = pt_right->left;

    if (pt->right != nullptr)
        pt->right->parent = pt;

    pt_right->parent = pt->parent;

    if (pt->parent == nullptr)
        root = pt_right;
    else if (pt == pt->parent->left)
        pt->parent->left = pt_right;
    else
        pt->parent->right = pt_right;

    pt_right->left = pt;
    pt->parent = pt_right;
}


void RedBlackTree::rotateRight(Node *&pt) {
    Node *pt_left = pt->left;
    pt->left = pt_left->right;

    if (pt->left != nullptr)
        pt->left->parent = pt;

    pt_left->parent = pt->parent;

    if (pt->parent == nullptr)
        root = pt_left;
    else if (pt == pt->parent->left)
        pt->parent->left = pt_left;
    else
        pt->parent->right = pt_left;

    pt_left->right = pt;
    pt->parent = pt_left;
}



// In `fixViolation`, we handle two main cases: 
// - Case A where the parent of the inserted node is the left child of the grandparent.
// - Case B where the parent is the right child of the grandparent.
// Each of these cases has sub-cases to handle the position of the newly inserted node relative to its parent and the color of its uncle node. The function ensures that after every insertion, the Red-Black Tree properties are maintained by appropriately rotating and changing the colors of the nodes.

void RedBlackTree::fixViolation(Node *&pt) {
    Node *parent_pt = nullptr;
    Node *grandparent_pt = nullptr;

    while ((pt != root) && (pt->color != BLACK) && (pt->parent->color == RED)) {
        parent_pt = pt->parent;
        grandparent_pt = pt->parent->parent;

        // Case A: Parent of pt is left child of Grandparent of pt
        if (parent_pt == grandparent_pt->left) {
            Node *uncle_pt = grandparent_pt->right;

            // Case 1: The uncle of pt is also red
            if (uncle_pt != nullptr && uncle_pt->color == RED) {
                grandparent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grandparent_pt;
            } else {
                // Case 2: pt is right child of its parent
                if (pt == parent_pt->right) {
                    rotateLeft(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }

                // Case 3: pt is left child of its parent
                rotateRight(grandparent_pt);
                swap(parent_pt->color, grandparent_pt->color);
                pt = parent_pt;
            }
        }
        // Case B: Parent of pt is right child of Grandparent of pt
        else {
            Node *uncle_pt = grandparent_pt->left;

            // Case 1: The uncle of pt is also red
            if (uncle_pt != nullptr && uncle_pt->color == RED) {
                grandparent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grandparent_pt;
            } else {
                // Case 2: pt is left child of its parent
                if (pt == parent_pt->left) {
                    rotateRight(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }

                // Case 3: pt is right child of its parent
                rotateLeft(grandparent_pt);
                swap(parent_pt->color, grandparent_pt->color);
                pt = parent_pt;
            }
        }
    }

    root->color = BLACK; // Ensure the root is always black
}




Node* RedBlackTree::BSTInsert(Node*& root, Node*& pt) {
    if (root == nullptr)
        return pt;

    if (pt->data < root->data) {
        root->left = BSTInsert(root->left, pt);
        root->left->parent = root;
    } else if (pt->data > root->data) {
        root->right = BSTInsert(root->right, pt);
        root->right->parent = root;
    }

    return root;
}

void RedBlackTree::inorderHelper(Node*& root) {
    if (root == nullptr)
        return;

    inorderHelper(root->left);
    cout << root->data << " ";
    inorderHelper(root->right);
}

void RedBlackTree::levelOrderHelper(Node*& root) {
    if (root == nullptr)
        return;

    queue<Node*> q;
    q.push(root);

    while (!q.empty()) {
        Node* temp = q.front();
        cout << temp->data << " ";
        q.pop();

        if (temp->left != nullptr)
            q.push(temp->left);
        if (temp->right != nullptr)
            q.push(temp->right);
    }
}

void RedBlackTree::fixInsertRBTree(Node*& pt) {
    Node *parent_pt = nullptr;
    Node *grand_parent_pt = nullptr;

    while ((pt != root) && (pt->color != BLACK) && (pt->parent->color == RED)) {
        parent_pt = pt->parent;
        grand_parent_pt = pt->parent->parent;

        /*  Case : A
            Parent of pt is left child of Grand-parent of pt */
        if (parent_pt == grand_parent_pt->left) {
            Node *uncle_pt = grand_parent_pt->right;

            /* Case : 1
               The uncle of pt is also red
               Only Recoloring required */
            if (uncle_pt != nullptr && uncle_pt->color == RED) {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grand_parent_pt;
            } else {
                /* Case : 2
                   pt is right child of its parent
                   Left-rotation required */
                if (pt == parent_pt->right) {
                    rotateLeft(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }

                /* Case : 3
                   pt is left child of its parent
                   Right-rotation required */
                rotateRight(grand_parent_pt);
                swap(parent_pt->color, grand_parent_pt->color);
                pt = parent_pt;
            }
        }

        /* Case : B
           Parent of pt is right child of Grand-parent of pt */
        else {
            Node *uncle_pt = grand_parent_pt->left;

            /*  Case : 1
                The uncle of pt is also red
                Only Recoloring required */
            if ((uncle_pt != nullptr) && (uncle_pt->color == RED)) {
                grand_parent_pt->color = RED;
                parent_pt->color = BLACK;
                uncle_pt->color = BLACK;
                pt = grand_parent_pt;
            } else {
                /* Case : 2
                   pt is left child of its parent
                   Right-rotation required */
                if (pt == parent_pt->left) {
                    rotateRight(parent_pt);
                    pt = parent_pt;
                    parent_pt = pt->parent;
                }

                /* Case : 3
                   pt is right child of its parent
                   Left-rotation required */
                rotateLeft(grand_parent_pt);
                swap(parent_pt->color, grand_parent_pt->color);
                pt = parent_pt;
            }
        }
    }

    root->color = BLACK;
}

int RedBlackTree::getColor(Node*& node) {
    if (node == nullptr)
        return BLACK;

    return node->color;
}

void RedBlackTree::setColor(Node*& node, int color) {
    if (node == nullptr)
        return;

    node->color = color;
}

Node* RedBlackTree::minValueNode(Node*& node) {
    Node* ptr = node;

    while (ptr->left != nullptr)
        ptr = ptr->left;

    return ptr;
}

Node* RedBlackTree::maxValueNode(Node*& node) {
    Node* ptr = node;

    while (ptr->right != nullptr)
        ptr = ptr->right;

    return ptr;
}

Node* RedBlackTree::deleteBST(Node *&root, int data) {
    if (root == nullptr)
        return root;

    if (data < root->data)
        return deleteBST(root->left, data);

    if (data > root->data)
        return deleteBST(root->right, data);

    if (root->left == nullptr || root->right == nullptr)
        return root;

    Node *temp = minValueNode(root->right);
    root->data = temp->data;
    return deleteBST(root->right, temp->data);
}

void RedBlackTree::fixDeleteRBTree(Node *&node) {
    if (node == nullptr)
        return;

    if (node == root) {
        root = nullptr;
        return;
    }

    if (getColor(node) == RED || getColor(node->left) == RED || getColor(node->right) == RED) {
        Node *child = node->left != nullptr ? node->left : node->right;

        if (node == node->parent->left) {
            node->parent->left = child;
            if (child != nullptr)
                child->parent = node->parent;
            setColor(child, BLACK);
            delete (node);
        } else {
            node->parent->right = child;
            if (child != nullptr)
                child->parent = node->parent;
            setColor(child, BLACK);
            delete (node);
        }
    } else {
        Node *sibling;
        Node *parent;
        Node *ptr = node;
        bool ptrIsBlack = true;

        while (ptr != root && ptrIsBlack) {
            parent = ptr->parent;
            if (ptr == parent->left) {
                sibling = parent->right;
                if (getColor(sibling) == RED) {
                    setColor(sibling, BLACK);
                    setColor(parent, RED);
                    rotateLeft(parent);
                    sibling = parent->right;
                }

                if (getColor(sibling->left) == BLACK && getColor(sibling->right) == BLACK) {
                    setColor(sibling, RED);
                    ptr = parent;
                    ptrIsBlack = (getColor(ptr) == BLACK);
                } else {
                    if (getColor(sibling->right) == BLACK) {
                        setColor(sibling->left, BLACK);
                        setColor(sibling, RED);
                        rotateRight(sibling);
                        sibling = parent->right;
                    }

                    setColor(sibling, getColor(parent));
                    setColor(parent, BLACK);
                    setColor(sibling->right, BLACK);
                    rotateLeft(parent);
                    break;
                }
            } else {
                sibling = parent->left;
                if (getColor(sibling) == RED) {
                    setColor(sibling, BLACK);
                    setColor(parent, RED);
                    rotateRight(parent);
                    sibling = parent->left;
                }

                if (getColor(sibling->right) == BLACK && getColor(sibling->left) == BLACK) {
                    setColor(sibling, RED);
                    ptr = parent;
                    ptrIsBlack = (getColor(ptr) == BLACK);
                } else {
                    if (getColor(sibling->left) == BLACK) {
                        setColor(sibling->right, BLACK);
                        setColor(sibling, RED);
                        rotateLeft(sibling);
                        sibling = parent->left;
                    }

                    setColor(sibling, getColor(parent));
                    setColor(parent, BLACK);
                    setColor(sibling->left, BLACK);
                    rotateRight(parent);
                    break;
                }
            }
        }

        if (ptr == ptr->parent->left)
            ptr->parent->left = nullptr;
        else
            ptr->parent->right = nullptr;

        delete (ptr);
        if (root != nullptr)
            root->color = BLACK;
    }
}


int main() {
    RedBlackTree tree;

    // Test cases: Inserting values
    tree.insert(7);
    tree.insert(3);
    tree.insert(18);
    tree.insert(10);
    tree.insert(22);
    tree.insert(8);
    tree.insert(11);
    tree.insert(26);
    tree.insert(5);
    tree.insert(2);
    tree.insert(6);
    tree.insert(13);
    tree.deleteNode(10);

    cout << "Inorder Traversal of Created Tree\n";
    tree.inorder();
    cout << "\n\nLevel Order Traversal of Created Tree\n";
    tree.levelOrder();

    return 0;
}
