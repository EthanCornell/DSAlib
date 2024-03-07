#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <queue>

template <typename T>
struct Node
{
    T value;
    std::unique_ptr<Node<T>> left;
    std::unique_ptr<Node<T>> right;

    Node(T val) : value(val), left(nullptr), right(nullptr) {}
};

template <typename T>
class BinaryTree
{
private:
    std::unique_ptr<Node<T>> root;

    // Helper to write the Graphviz representation of the tree to a stream
    void graphvizHelper(const std::unique_ptr<Node<T>> &node, std::ostream &out) const
    {
        if (!node)
            return;
        if (node->left)
        {
            out << "    \"" << node->value << "\" -> \"" << node->left->value << "\";\n";
            graphvizHelper(node->left, out);
        }
        if (node->right)
        {
            out << "    \"" << node->value << "\" -> \"" << node->right->value << "\";\n";
            graphvizHelper(node->right, out);
        }
    }

    void insertHelper(std::unique_ptr<Node<T>> &node, T val)
    {
        if (!node)
        {
            node = std::make_unique<Node<T>>(val);
            return;
        }
        if (val < node->value)
        {
            insertHelper(node->left, val);
        }
        else
        {
            insertHelper(node->right, val);
        }
    }

    // Helper function to find and remove the smallest node in a subtree
    std::unique_ptr<Node<T>> &getMinNode(std::unique_ptr<Node<T>> &node)
    {
        return (node->left) ? getMinNode(node->left) : node;
    }

    void removeHelper(std::unique_ptr<Node<T>> &node, T val)
    {
        if (!node)
        {
            return; // Value not found
        }
        if (val < node->value)
        {
            removeHelper(node->left, val);
        }
        else if (val > node->value)
        {
            removeHelper(node->right, val);
        }
        else
        {
            // Node with only one child or no child
            if (!node->left)
            {
                node = std::move(node->right);
            }
            else if (!node->right)
            {
                node = std::move(node->left);
            }
            else
            {
                // Node with two children, get the inorder successor (smallest in the right subtree)
                std::unique_ptr<Node<T>> &minNode = getMinNode(node->right);
                node->value = minNode->value;
                removeHelper(minNode, minNode->value); // Remove the inorder successor
            }
        }
    }

    void printInOrderHelper(const std::unique_ptr<Node<T>> &node) const
    {
        if (!node)
            return;
        printInOrderHelper(node->left);
        std::cout << node->value << " ";
        printInOrderHelper(node->right);
    }

    // Helper function for post-order traversal
    void postOrderHelper(const std::unique_ptr<Node<T>> &node) const
    {
        if (!node)
            return;

        // Recur on the left subtree
        postOrderHelper(node->left);

        // Recur on the right subtree
        postOrderHelper(node->right);

        // Visit the root (current node)
        std::cout << node->value << " ";
    }

public:
    BinaryTree() : root(nullptr) {}

    void insert(T val)
    {
        insertHelper(root, val);
    }

    bool search(T val) const
    {
        Node<T> *current = root.get();
        while (current != nullptr)
        {
            if (val == current->value)
            {
                return true;
            }
            else if (val < current->value)
            {
                current = current->left.get();
            }
            else
            {
                current = current->right.get();
            }
        }
        return false;
    }

    void remove(T val)
    {
        removeHelper(root, val);
    }

    void printLevelOrder() const
    {
        if (!root)
            return;
        std::queue<const Node<T> *> q;
        q.push(root.get());

        while (!q.empty())
        {
            int size = q.size();
            for (int i = 0; i < size; ++i)
            {
                const Node<T> *current = q.front();
                q.pop();
                std::cout << current->value << " ";
                if (current->left)
                    q.push(current->left.get());
                if (current->right)
                    q.push(current->right.get());
            }
            std::cout << std::endl; // New line for a new level
        }
    }

    void printInOrder() const
    {
        printInOrderHelper(root);
        std::cout << std::endl;
    }

    // Public function for post-order traversal
    void postOrder() const
    {
        postOrderHelper(root);
        std::cout << std::endl;
    }

    // Function to generate the Graphviz representation of the tree
    void printGraphviz() const
    {
        std::ostringstream out;
        out << "digraph BinaryTree {\n";
        graphvizHelper(root, out);
        out << "}\n";

        std::ofstream file("tree.gv");
        file << out.str();
        file.close();

        // This system call is to invoke Graphviz to create an image file from the .gv file
        std::system("dot -Tpng tree.gv -o tree.png");
    }
};

// Usage of BinaryTree and printGraphviz()

int main()
{
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(8);
    tree.insert(1);
    tree.insert(4);

    tree.insert(13);
    tree.insert(12);
    tree.insert(38);
    tree.insert(30);
    tree.insert(53);
    tree.insert(79);
    tree.insert(65);

    std::cout << "Before removal: ";
    tree.printInOrder();

    tree.remove(38);

    std::cout << "Tree in In-Order Traversal: " << std::endl;
    tree.printInOrder(); // This will print the tree in sorted order

    std::cout << "Post-order traversal: ";
    tree.postOrder();

    std::cout << "Tree in Level-Order Traversal: " << std::endl;
    tree.printLevelOrder(); // This will print the tree level by level

    tree.printGraphviz(); // This will create a .gv file and then a .png image of the tree

    return 0;
}
// g++ -pg -fsanitize=address -g  -std=c++17 BST_smartptr2.cpp -o ./BST_smartptr2 && ./BST_smartptr2