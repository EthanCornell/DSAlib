#include <iostream>
#include <vector>
using namespace std;

// Structure to represent an interval
struct Interval {
    int start, end;
};

// Structure to represent a node in Interval Tree
struct TreeNode {
    Interval *interval; // An interval
    int maxEnd; // Maximum end value in the subtree rooted with this node
    TreeNode *left, *right;

    TreeNode(Interval i) : interval(new Interval(i)), maxEnd(i.end), left(nullptr), right(nullptr) {}
};

// Class for Interval Tree
class IntervalTree {
public:
    TreeNode *root = nullptr;

    // Utility function to insert a new interval search tree node
    TreeNode *insert(TreeNode *node, Interval interval) {
        // Base case: tree is empty, new node becomes root
        if (node == nullptr) return new TreeNode(interval);

        // Get low value of interval at root
        int l = node->interval->start;

        // If root's low value is smaller, then new interval goes to left subtree
        if (interval.start < l) {
            node->left = insert(node->left, interval);
        } else { // Else, right subtree
            node->right = insert(node->right, interval);
        }

        // Update the maxEnd of this ancestor if needed
        if (node->maxEnd < interval.end) {
            node->maxEnd = interval.end;
        }

        return node;
    }

    // Function to insert a new interval
    void insert(Interval interval) {
        root = insert(root, interval);
    }

    // Utility function to check if given two intervals overlap
    bool doOverlap(Interval i1, Interval i2) {
        if (i1.start <= i2.end && i2.start <= i1.end) return true;
        return false;
    }

    // Function to search overlapping intervals and print them
    void searchOverlappingIntervals(TreeNode *node, Interval query) {
        // Base case
        if (node == nullptr) return;

        // If node's interval overlaps with query, print it
        if (doOverlap(*(node->interval), query)) {
            cout << "[" << node->interval->start << ", " << node->interval->end << "] ";
        }

        // If left child of node is not null and the maxEnd in left child
        // is greater than or equal to query's start, then query may
        // overlap with an interval in left subtree
        if (node->left != nullptr && node->left->maxEnd >= query.start) {
            searchOverlappingIntervals(node->left, query);
        }

        // Always check in right subtree
        searchOverlappingIntervals(node->right, query);
    }

    // Function to search a given query interval
    void search(Interval query) {
        cout << "Overlapping intervals with [" << query.start << ", " << query.end << "]: ";
        searchOverlappingIntervals(root, query);
        cout << endl;
    }
};

int main() {
    IntervalTree tree;
    // Interval intervals[] = {{11, 21}, {9, 33}, {17, 19}, {5, 29}, {11, 18}, {30, 48}, {60, 88}};
    
    tree.insert({0, 20});
    tree.insert({10, 30});
    tree.insert({27, 49});
    tree.insert({5, 20});
    tree.insert({12, 18});
    tree.insert({30, 40});

    Interval query = {6, 7};
    tree.search(query); // Search for overlapping intervals with this query
    

    return 0;
}

// g++ -pg -fsanitize=address -g ./intervalTree2.cpp -o iT2 -O3  && ./iT2