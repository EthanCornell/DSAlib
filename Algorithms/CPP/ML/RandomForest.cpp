#include <vector>
#include <unordered_map>
#include <string>
#include <limits>
#include <iostream>
#include <unordered_map>
#include <algorithm> // For std::max_element
#include <iostream>
#include <random>
#include <numeric>

// Include the header that defines the object class, which is part of the library.

struct DecisionTreeNode
{
    int splitFeature = -1; // -1 indicates a leaf node
    double splitValue = 0;
    int label = -1; // Used only for leaf nodes to store the class label
    DecisionTreeNode *left = nullptr;
    DecisionTreeNode *right = nullptr;
};

class DecisionTree
{
private:
    DecisionTreeNode *root;

    // Function to calculate the Gini impurity for a split
    double calculateGini(const std::vector<int> &labels)
    {
        std::unordered_map<int, int> labelCounts;
        for (int label : labels)
        {
            labelCounts[label]++;
        }
        double impurity = 1.0;
        for (auto &pair : labelCounts)
        {
            double prob = pair.second / static_cast<double>(labels.size());
            impurity -= prob * prob;
        }
        return impurity;
    }

    // Recursive function to build the tree
    DecisionTreeNode* buildTree(const std::vector<std::vector<double>>& data, const std::vector<int>& labels, const std::vector<int>& featureIndices, int depth = 0, int maxDepth = 10) {
        // Stopping conditions
        if (depth >= maxDepth || data.size() <= 2) {
            // Return a leaf node with the most common label
            std::unordered_map<int, int> labelCounts;
            for (int label : labels) {
                labelCounts[label]++;
            }
            int mostCommonLabel = std::max_element(labelCounts.begin(), labelCounts.end(), 
                                [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                                    return a.second < b.second; })->first;
            DecisionTreeNode* node = new DecisionTreeNode();
            node->label = mostCommonLabel;
            return node;
        }

        int bestFeatureIndex = -1;
        double bestSplitValue = 0;
        double bestImpurity = std::numeric_limits<double>::max();

        // Iterate over the feature indices to find the best split
        for (int index : featureIndices) {
            std::vector<double> featureValues;
            for (const auto& row : data) {
                featureValues.push_back(row[index]);
            }
            std::sort(featureValues.begin(), featureValues.end());

            double splitValue = featureValues[featureValues.size() / 2]; // Median as potential split value

            std::vector<int> leftLabels, rightLabels;
            for (size_t i = 0; i < data.size(); ++i) {
                if (data[i][index] < splitValue) {
                    leftLabels.push_back(labels[i]);
                } else {
                    rightLabels.push_back(labels[i]);
                }
            }

            double impurity = (calculateGini(leftLabels) * leftLabels.size() + calculateGini(rightLabels) * rightLabels.size()) / data.size();

            if (impurity < bestImpurity) {
                bestFeatureIndex = index;
                bestSplitValue = splitValue;
                bestImpurity = impurity;
            }
        }

        if (bestFeatureIndex == -1) return nullptr; // No improvement found

        std::vector<std::vector<double>> leftData, rightData;
        std::vector<int> leftLabels, rightLabels;
        for (size_t i = 0; i < data.size(); ++i) {
            if (data[i][bestFeatureIndex] < bestSplitValue) {
                leftData.push_back(data[i]);
                leftLabels.push_back(labels[i]);
            } else {
                rightData.push_back(data[i]);
                rightLabels.push_back(labels[i]);
            }
        }

        DecisionTreeNode* node = new DecisionTreeNode();
        node->splitFeature = bestFeatureIndex;
        node->splitValue = bestSplitValue;

        if (!leftData.empty()) {
            node->left = buildTree(leftData, leftLabels, featureIndices, depth + 1, maxDepth);
        }
        if (!rightData.empty()) {
            node->right = buildTree(rightData, rightLabels, featureIndices, depth + 1, maxDepth);
        }

        return node;
    }

public:
    DecisionTree() : root(nullptr) {}

    ~DecisionTree()
    {
        // Destructor to free memory; implement this to avoid memory leaks
    }

    void train(const std::vector<std::vector<double>> &data, const std::vector<int> &labels, const std::vector<int> &featureIndices)
    {
        int maxDepth = 10; // Or another appropriate value
        root = buildTree(data, labels, featureIndices, 0, maxDepth);
    }

    // Add prediction function here if needed
    int predict(const std::vector<double> &input)
    {
        DecisionTreeNode *node = root;
        while (node->splitFeature != -1)
        { // Not a leaf node
            if (input[node->splitFeature] < node->splitValue)
            {
                node = node->left;
            }
            else
            {
                node = node->right;
            }
        }
        return node->label;
    }
};

class RandomForest
{
private:
    int n_trees;
    std::vector<DecisionTree> trees;

public:
    RandomForest(int n_trees) : n_trees(n_trees)
    {
        trees.resize(n_trees);
    }

    void train(const std::vector<std::vector<double>> &data, const std::vector<int> &labels)
    {
        std::random_device rd;
        std::mt19937 g(rd());

        for (auto &tree : trees)
        {
            // Bootstrap the data
            std::vector<std::vector<double>> bootstrapData;
            std::vector<int> bootstrapLabels;
            for (size_t i = 0; i < data.size(); ++i)
            {
                int index = std::uniform_int_distribution<>(0, data.size() - 1)(g);
                bootstrapData.push_back(data[index]);
                bootstrapLabels.push_back(labels[index]);
            }

            // Randomly select features for this tree
            std::vector<int> featureIndices(data[0].size());
            std::iota(featureIndices.begin(), featureIndices.end(), 0); // Fill with 0, 1, ..., n_features - 1
            std::shuffle(featureIndices.begin(), featureIndices.end(), g);
            featureIndices.resize(std::sqrt(featureIndices.size())); // Keep only a subset, e.g., sqrt(total features)

            // Train the tree on the bootstrapped data and selected features
            tree.train(bootstrapData, bootstrapLabels, featureIndices);
        }
    }

    int predict(const std::vector<double> &input)
    {
        std::unordered_map<int, int> classVotes;
        for (auto &tree : trees)
        {
            int prediction = tree.predict(input);
            classVotes[prediction]++;
        }

        // Find the class with the most votes
        int maxVotes = 0;
        int predictedClass = -1;
        for (auto &vote : classVotes)
        {
            if (vote.second > maxVotes)
            {
                maxVotes = vote.second;
                predictedClass = vote.first;
            }
        }

        return predictedClass;
    }
};

#include <iostream>
// Main function to demonstrate usage
int main()
{
    // Example dataset and labels (simple binary classification problem)
    std::vector<std::vector<double>> data = {
        {2.5, 2.4, 0.1}, {0.5, 0.7, 0.2}, {2.2, 2.9, 0.8}, {1.9, 2.2, 0.5}, {3.1, 3.0, 1.2}, {2.3, 2.7, 1.0}, {2, 1.6, 0.2}, {1, 1.1, 0.3}, {1.5, 1.6, 0.6}, {1.1, 0.9, 0.1}};
    std::vector<int> labels = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1};

    // Instantiate RandomForest with a specified number of trees
    RandomForest forest(5);

    // Feature indices (assuming all features are used for simplicity)
    std::vector<int> featureIndices(data[0].size());
    std::iota(featureIndices.begin(), featureIndices.end(), 0);

    // Train the RandomForest model
    forest.train(data, labels);

    // Example inputs for prediction
    std::vector<std::vector<double>> testInputs = {
        {2.0, 2.0, 0.5},
        {1.0, 1.0, 0.1},
        {3.0, 3.0, 1.0}};

    // Make predictions for the test inputs
    for (const auto &input : testInputs)
    {
        int predictedLabel = forest.predict(input);
        std::cout << "Predicted label for input (" << input[0] << ", " << input[1] << ", " << input[2] << "): " << predictedLabel << std::endl;
    }

    return 0;
}

// g++ -pg -fopenmp -std=c++17 -o para paraTest.cpp -lpthread -O3 && ./para
// g++ -pg  -std=c++17 -o RF RandomForest.cpp  -O3 && ./RF