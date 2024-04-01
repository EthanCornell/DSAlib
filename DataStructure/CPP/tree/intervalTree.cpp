#include <iostream>
#include <memory>
using namespace std;

// Define the structure for an interval
struct Interval
{
    int low, high;
};

// Define the structure for a tree node
struct TreeNode
{
    unique_ptr<Interval> i; // Smart pointer to interval
    int maxHigh;            // Maximum high value in the subtree rooted with this node
    unique_ptr<TreeNode> left, right;
    TreeNode(Interval i) : i(make_unique<Interval>(i)), maxHigh(i.high) {}
};

// A utility function to insert a new Interval and update the maxHigh
unique_ptr<TreeNode> insert(unique_ptr<TreeNode> root, Interval i)
{
    // Base case: tree is empty, new node becomes root
    if (!root)
        return make_unique<TreeNode>(i);

    // Get low value of interval at root
    int l = root->i->low;

    // If root's low value is smaller, then new interval goes to left subtree
    if (i.low < l)
        root->left = insert(move(root->left), i);
    else // Else, right subtree
        root->right = insert(move(root->right), i);

    // Update the maxHigh of this ancestor, if needed
    if (root->maxHigh < i.high)
        root->maxHigh = i.high;

    return root;
}

// A utility function to check if given two intervals overlap
bool doOverlap(const Interval &i1, const Interval &i2)
{
    return (i1.low <= i2.high && i2.low <= i1.high);
}

// The main function that searches a given interval i in a given Interval Tree.
Interval *overlapSearch(const unique_ptr<TreeNode> &root, Interval i)
{
    // Base Case, tree is empty
    if (!root)
        return nullptr;

    // If given interval overlaps with root
    if (doOverlap(*(root->i), i))
        return root->i.get();

    // If left child of root is present and max of left child is
    // greater than or equal to given interval, then i may overlap with an interval in left subtree
    if (root->left && root->left->maxHigh >= i.low)
        return overlapSearch(root->left, i);

    // Else interval can only overlap with right subtree
    return overlapSearch(root->right, i);
}

// A utility function to print the interval tree
void printTree(const unique_ptr<TreeNode> &root)
{
    if (!root)
        return;

    // Print left subtree
    printTree(root->left);

    // Print this node's interval and maxHigh
    cout << "[" << root->i->low << ", " << root->i->high << "] max = " << root->maxHigh << endl;

    // Print right subtree
    printTree(root->right);
}

int main()
{
    Interval intervals[] = {{11, 21}, {9, 33}, {17, 19}, {5, 29}, {11, 18}, {30, 48}, {60, 88}};
    int n = sizeof(intervals) / sizeof(intervals[0]);
    unique_ptr<TreeNode> root = nullptr;
    for (int i = 0; i < n; i++)
        root = insert(move(root), intervals[i]);

    Interval x = {6, 7};

    cout << "Searching for interval [" << x.low << "," << x.high << "]" << endl;
    Interval *res = overlapSearch(root, x);
    if (res == nullptr)
        cout << "No Overlapping Interval";
    else
        cout << "Overlaps with [" << res->low << ", " << res->high << "]" << endl;

    // After the tree has been created and populated, call printTree
    cout << "Interval Tree:" << endl;
    printTree(root);

    return 0;
}


// int main()
// {
//     // Let us create interval tree shown in above figure
//     Interval ints[] = {{15, 20}, {10, 30}, {17, 19},
//         {5, 20}, {12, 15}, {30, 40}
//     };
//     int n = sizeof(ints)/sizeof(ints[0]);
//     unique_ptr<TreeNode> root = nullptr;
//     for (int i = 0; i < n; i++)
//         root = insert(move(root), ints[i]);
 
//     cout << "Inorder traversal of constructed Interval Tree is\n";
//     printTree(root);
 
//     Interval x = {6, 7};
 
//     cout << "\nSearching for interval [" << x.low << "," << x.high << "]";
//     Interval *res = overlapSearch(root, x);
//     if (res == NULL)
//         cout << "\nNo Overlapping Interval";
//     else
//         cout << "\nOverlaps with [" << res->low << ", " << res->high << "]";
//     return 0;
// }

// g++ -pg -fsanitize=address -g ./intervalTree.cpp -o iT -O3  && ./iT