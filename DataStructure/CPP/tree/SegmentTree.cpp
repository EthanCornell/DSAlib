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

#include <vector>
#include <iostream>

// Data Structure:

// Use a tree-like structure where each node contains aggregate information (like sum, min, or max) for a range of elements.
// The tree is usually implemented as an array for efficiency.
// Build Function:

// Recursively build the tree from the given array.
// Each leaf represents an element of the array, and each internal node represents the sum of a segment.
// Update Function:

// To update a value, change the value in the leaf node and then recursively update the value in its parent nodes.
// Query Function:

// Query the sum of a given range by traversing the tree and combining the results from relevant segments.
// Time Complexity:

// Building the tree takes O(n) time.
// Queries and updates take O(log n) time.
// Space Complexity:

// O(n) space is required to store the tree.
// Main Function and Test Cases:

// Test the implementation with various scenarios, including updating elements and querying different ranges.
class SegmentTree {
    std::vector<int> tree;
    int n;

    // Recursive function to build the segment tree
    void buildTree(const std::vector<int>& nums, int node, int start, int end) {
        if (start == end) {
            // Leaf node will hold the element in the array
            tree[node] = nums[start];
        } else {
            int mid = (start + end) / 2;
            // Recursively build the left and right children
            buildTree(nums, 2 * node, start, mid);
            buildTree(nums, 2 * node + 1, mid + 1, end);
            // Internal node will hold the sum of both its children
            tree[node] = tree[2 * node] + tree[2 * node + 1];
        }
    }

    // Recursive function to update a value
    void updateTree(int node, int start, int end, int idx, int val) {
        if (start == end) {
            // Leaf node. Update the value
            tree[node] = val;
        } else {
            int mid = (start + end) / 2;
            if (idx <= mid) {
                // If idx is in the left child
                updateTree(2 * node, start, mid, idx, val);
            } else {
                // If idx is in the right child
                updateTree(2 * node + 1, mid + 1, end, idx, val);
            }
            // Update the current node based on children's values
            tree[node] = tree[2 * node] + tree[2 * node + 1];
        }
    }

    // Recursive function to query the sum of a range
    int queryTree(int node, int start, int end, int l, int r) {
        if (r < start || end < l) {
            // If the range represented by a node is completely outside the given range
            return 0;
        }
        if (l <= start && end <= r) {
            // If the range represented by a node is completely inside the given range
            return tree[node];
        }
        // If the range represented by a node is partially inside and partially outside the given range
        int mid = (start + end) / 2;
        int p1 = queryTree(2 * node, start, mid, l, r);
        int p2 = queryTree(2 * node + 1, mid + 1, end, l, r);
        return p1 + p2;
    }

public:
    SegmentTree(const std::vector<int>& nums) {
        n = nums.size();
        tree.resize(4 * n);
        buildTree(nums, 1, 0, n - 1);
    }

    void update(int idx, int val) {
        // Update a value in the array and rebuild the segment
        updateTree(1, 0, n - 1, idx, val);
    }

    int sumRange(int l, int r) {
        // Query the sum of the given range
        return queryTree(1, 0, n - 1, l, r);
    }
};

int main() {
    std::vector<int> nums = {1, 3, 5, 7, 9, 11};
    SegmentTree segTree(nums);

    // Test 1: Querying the sum of a range.
    // Expectation: Sum of values from index 1 to 3 (3 + 5 + 7) = 15
    std::cout << "Sum of range [1, 3]: " << segTree.sumRange(1, 3) << std::endl;

    // Test 2: Updating a value.
    // Update: Change value at index 2 to 10.
    segTree.update(2, 10);

    // Test 3: Querying the sum of a range after an update.
    // Expectation: Sum of values from index 1 to 3 (3 + 10 + 7) = 20
    std::cout << "Sum of range [1, 3] after update: " << segTree.sumRange(1, 3) << std::endl;

    return 0;
}


