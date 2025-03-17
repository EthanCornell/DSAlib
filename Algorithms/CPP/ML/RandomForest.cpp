#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>
#include <algorithm>
#include <random>
#include <numeric>
#include <memory>

// Node structure for decision tree using smart pointers
struct DecisionTreeNode {
    int splitFeature = -1; // -1 indicates a leaf node
    double splitValue = 0;
    int label = -1; // For leaf nodes
    std::unique_ptr<DecisionTreeNode> left = nullptr;
    std::unique_ptr<DecisionTreeNode> right = nullptr;
};

class DecisionTree {
private:
    std::unique_ptr<DecisionTreeNode> root;

    // Calculate the Gini impurity for a set of labels
    double calculateGini(const std::vector<int>& labels) {
        std::unordered_map<int, int> labelCounts;
        for (int label : labels) {
            labelCounts[label]++;
        }
        double impurity = 1.0;
        for (const auto& pair : labelCounts) {
            double prob = pair.second / static_cast<double>(labels.size());
            impurity -= prob * prob;
        }
        return impurity;
    }

    // Check if all labels are identical (pure node)
    bool isPure(const std::vector<int>& labels) {
        return std::all_of(labels.begin(), labels.end(),
                           [&](int label) { return label == labels[0]; });
    }

    // Recursively build the decision tree
    std::unique_ptr<DecisionTreeNode> buildTree(const std::vector<std::vector<double>>& data,
                                                  const std::vector<int>& labels,
                                                  const std::vector<int>& featureIndices,
                                                  int depth = 0,
                                                  int maxDepth = 10) {
        // Stop if maximum depth is reached, dataset is too small, or the node is pure.
        if (depth >= maxDepth || data.size() <= 2 || isPure(labels)) {
            auto node = std::make_unique<DecisionTreeNode>();
            std::unordered_map<int, int> labelCounts;
            for (int label : labels) {
                labelCounts[label]++;
            }
            int mostCommonLabel = std::max_element(labelCounts.begin(), labelCounts.end(),
                                                   [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                                                       return a.second < b.second;
                                                   })->first;
            node->label = mostCommonLabel;
            return node;
        }

        int bestFeatureIndex = -1;
        double bestSplitValue = 0;
        double bestImpurity = std::numeric_limits<double>::max();

        // Iterate over features to find the best split
        for (int index : featureIndices) {
            std::vector<double> featureValues;
            for (const auto& row : data) {
                featureValues.push_back(row[index]);
            }
            std::sort(featureValues.begin(), featureValues.end());
            double splitValue = featureValues[featureValues.size() / 2];

            std::vector<int> leftLabels, rightLabels;
            for (size_t i = 0; i < data.size(); ++i) {
                if (data[i][index] < splitValue) {
                    leftLabels.push_back(labels[i]);
                } else {
                    rightLabels.push_back(labels[i]);
                }
            }
            // Skip if either side is empty
            if (leftLabels.empty() || rightLabels.empty())
                continue;

            double impurity = (calculateGini(leftLabels) * leftLabels.size() +
                                calculateGini(rightLabels) * rightLabels.size()) / data.size();

            if (impurity < bestImpurity) {
                bestImpurity = impurity;
                bestFeatureIndex = index;
                bestSplitValue = splitValue;
            }
        }

        // If no valid split is found, return a leaf node
        if (bestFeatureIndex == -1) {
            auto node = std::make_unique<DecisionTreeNode>();
            std::unordered_map<int, int> labelCounts;
            for (int label : labels) {
                labelCounts[label]++;
            }
            int mostCommonLabel = std::max_element(labelCounts.begin(), labelCounts.end(),
                                                   [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                                                       return a.second < b.second;
                                                   })->first;
            node->label = mostCommonLabel;
            return node;
        }

        // Partition the data based on the best split
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

        auto node = std::make_unique<DecisionTreeNode>();
        node->splitFeature = bestFeatureIndex;
        node->splitValue = bestSplitValue;

        if (!leftData.empty())
            node->left = buildTree(leftData, leftLabels, featureIndices, depth + 1, maxDepth);
        if (!rightData.empty())
            node->right = buildTree(rightData, rightLabels, featureIndices, depth + 1, maxDepth);

        return node;
    }

public:
    DecisionTree() : root(nullptr) {}

    void train(const std::vector<std::vector<double>>& data,
               const std::vector<int>& labels,
               const std::vector<int>& featureIndices,
               int maxDepth = 10) {
        root = buildTree(data, labels, featureIndices, 0, maxDepth);
    }

    int predict(const std::vector<double>& input) const {
        DecisionTreeNode* node = root.get();
        while (node && node->splitFeature != -1) {
            if (input[node->splitFeature] < node->splitValue)
                node = node->left.get();
            else
                node = node->right.get();
        }
        return node ? node->label : -1;
    }
};

class RandomForest {
private:
    int n_trees;
    std::vector<DecisionTree> trees;

public:
    RandomForest(int n_trees) : n_trees(n_trees) {
        trees.resize(n_trees);
    }

    void train(const std::vector<std::vector<double>>& data, const std::vector<int>& labels) {
        std::random_device rd;
        std::mt19937 g(rd());

        for (auto& tree : trees) {
            // Bootstrap the data
            std::vector<std::vector<double>> bootstrapData;
            std::vector<int> bootstrapLabels;
            for (size_t i = 0; i < data.size(); ++i) {
                int index = std::uniform_int_distribution<>(0, data.size() - 1)(g);
                bootstrapData.push_back(data[index]);
                bootstrapLabels.push_back(labels[index]);
            }

            // Randomly select features for this tree
            std::vector<int> featureIndices(data[0].size());
            std::iota(featureIndices.begin(), featureIndices.end(), 0);
            std::shuffle(featureIndices.begin(), featureIndices.end(), g);
            featureIndices.resize(std::max(1, static_cast<int>(std::sqrt(featureIndices.size()))));

            // Train the tree on the bootstrapped data and selected features
            tree.train(bootstrapData, bootstrapLabels, featureIndices);
        }
    }

    int predict(const std::vector<double>& input) {
        std::unordered_map<int, int> classVotes;
        for (auto& tree : trees) {
            int prediction = tree.predict(input);
            classVotes[prediction]++;
        }

        int maxVotes = 0;
        int predictedClass = -1;
        for (auto& vote : classVotes) {
            if (vote.second > maxVotes) {
                maxVotes = vote.second;
                predictedClass = vote.first;
            }
        }
        return predictedClass;
    }
};

#include <iostream>

int main() {
    // Example dataset 
    std::vector<std::vector<double>> data = {
        {2.5, 2.4, 0.1}, {0.5, 0.7, 0.2}, {2.2, 2.9, 0.8}, {1.9, 2.2, 0.5},
        {3.1, 3.0, 1.2}, {2.3, 2.7, 1.0}, {2.0, 1.6, 0.2}, {1.0, 1.1, 0.3},
        {1.5, 1.6, 0.6}, {1.1, 0.9, 0.1}
    };
    std::vector<int> labels = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1};

    // Instantiate RandomForest with 5 trees
    RandomForest forest(5);

    // Train the RandomForest model
    forest.train(data, labels);

    // Example test inputs for prediction
    std::vector<std::vector<double>> testInputs = {
        {2.0, 2.0, 0.5},
        {1.0, 1.0, 0.1},
        {3.0, 3.0, 1.0}
    };

    // Make and display predictions
    for (const auto& input : testInputs) {
        int predictedLabel = forest.predict(input);
        std::cout << "Predicted label for input ("
                  << input[0] << ", " << input[1] << ", " << input[2]
                  << "): " << predictedLabel << std::endl;
    }
    return 0;
}

// g++ -pg -fopenmp -std=c++17 -o para paraTest.cpp -lpthread -O3 && ./para
// g++ -pg  -std=c++17 -o RF RandomForest.cpp  -O3 && ./RF
