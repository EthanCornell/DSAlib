#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <limits>
#include <random>
#include <memory> // For std::unique_ptr
#include <thread>
#include <future>
#include <omp.h>

struct DecisionTreeNode
{
    int splitFeature = -1; // -1 indicates a leaf node
    double splitValue = 0;
    int label = -1; // Used only for leaf nodes to store the class label
    std::unique_ptr<DecisionTreeNode> left = nullptr;
    std::unique_ptr<DecisionTreeNode> right = nullptr;
};

class DecisionTree
{
private:
    std::unique_ptr<DecisionTreeNode> root;

    double calculateGini(const std::vector<int> &labels)
    {
        std::unordered_map<int, int> labelCounts;
#pragma omp parallel for
        for (int label : labels)
        {
            labelCounts[label]++;
        }
        double impurity = 1.0;

        for (const auto &pair : labelCounts)
        {
            double prob = pair.second / static_cast<double>(labels.size());
            impurity -= prob * prob;
        }
        return impurity;
    }

    std::unique_ptr<DecisionTreeNode> buildTree(const std::vector<std::vector<double>> &data, const std::vector<int> &labels, const std::vector<int> &featureIndices, int depth = 0, int maxDepth = 10)
    {
        if (depth >= maxDepth || data.size() <= 2)
        {
            auto node = std::make_unique<DecisionTreeNode>();
            std::unordered_map<int, int> labelCounts;
            for (int label : labels)
            {
                labelCounts[label]++;
            }
#pragma omp max_element
            int mostCommonLabel = std::max_element(labelCounts.begin(), labelCounts.end(),
                                                   [](const std::pair<int, int> &a, const std::pair<int, int> &b)
                                                   { return a.second < b.second; })
                                      ->first;
            node->label = mostCommonLabel;
            return node;
        }

        int bestFeatureIndex = -1;
        double bestSplitValue = 0;
        double bestImpurity = std::numeric_limits<double>::max();

#pragma omp parallel for
        for (int index : featureIndices)
        {
            std::vector<double> featureValues;
            for (const auto &row : data)
            {
                featureValues.push_back(row[index]);
            }
            std::sort(featureValues.begin(), featureValues.end());

            double splitValue = featureValues[featureValues.size() / 2];

            std::vector<int> leftLabels, rightLabels;
            for (size_t i = 0; i < data.size(); ++i)
            {
                if (data[i][index] < splitValue)
                {
                    leftLabels.push_back(labels[i]);
                }
                else
                {
                    rightLabels.push_back(labels[i]);
                }
            }

#pragma omp calculateGini
            double impurity = (calculateGini(leftLabels) * leftLabels.size() + calculateGini(rightLabels) * rightLabels.size()) / data.size();

            if (impurity < bestImpurity)
            {
                bestFeatureIndex = index;
                bestSplitValue = splitValue;
                bestImpurity = impurity;
            }
        }

        if (bestFeatureIndex == -1)
            return nullptr;

        std::vector<std::vector<double>> leftData, rightData;
        std::vector<int> leftLabels, rightLabels;
#pragma omp for
        for (size_t i = 0; i < data.size(); ++i)
        {
            if (data[i][bestFeatureIndex] < bestSplitValue)
            {
                leftData.push_back(data[i]);
                leftLabels.push_back(labels[i]);
            }
            else
            {
                rightData.push_back(data[i]);
                rightLabels.push_back(labels[i]);
            }
        }

        auto node = std::make_unique<DecisionTreeNode>();
        node->splitFeature = bestFeatureIndex;
        node->splitValue = bestSplitValue;

        if (!leftData.empty())
        {
            node->left = buildTree(leftData, leftLabels, featureIndices, depth + 1, maxDepth);
        }
        if (!rightData.empty())
        {
            node->right = buildTree(rightData, rightLabels, featureIndices, depth + 1, maxDepth);
        }

        return node;
    }

public:
    DecisionTree() : root(nullptr) {}

    void train(const std::vector<std::vector<double>> &data, const std::vector<int> &labels, const std::vector<int> &featureIndices)
    {
#pragma omp buildTree
        root = buildTree(data, labels, featureIndices, 0, 10); // Adjust maxDepth as needed
    }

    int predict(const std::vector<double> &input)
    {
        DecisionTreeNode *node = root.get();
        while (node && node->splitFeature != -1)
        {
            if (input[node->splitFeature] < node->splitValue)
            {
                node = node->left.get();
            }
            else
            {
                node = node->right.get();
            }
        }
        return node ? node->label : -1; // Return -1 if prediction is not possible
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

    //     void train(const std::vector<std::vector<double>> &data, const std::vector<int> &labels)
    //     {
    //         std::vector<std::future<void>> futures;
    //         std::random_device rd;
    //         std::mt19937 g(rd());

    // #pragma omp parallel for // Parallelize the loop across trees
    //         for (auto &tree : trees)
    //         {
    //             futures.push_back(std::async(std::launch::async, [&tree, &data, &labels, &g]()
    //                                          {
    //                 std::vector<std::vector<double>> bootstrapData;
    //                 std::vector<int> bootstrapLabels;
    // #pragma omp for
    //                 for (size_t i = 0; i < data.size(); ++i) {
    //                     int index = std::uniform_int_distribution<>(0, data.size() - 1)(g);
    //                     bootstrapData.push_back(data[index]);
    //                     bootstrapLabels.push_back(labels[index]);
    //                 }

    //                 std::vector<int> featureIndices(data[0].size());
    //                 std::iota(featureIndices.begin(), featureIndices.end(), 0);
    //                 std::shuffle(featureIndices.begin(), featureIndices.end(), g);
    //                 featureIndices.resize(std::sqrt(featureIndices.size()));
    // #pragma omp train
    //                 tree.train(bootstrapData, bootstrapLabels, featureIndices); }));
    //         }

    //         // Wait for all futures to complete
    //         for (auto &future : futures)
    //         {
    //             future.wait();
    //         }
    //     }

    void train(const std::vector<std::vector<double>> &data, const std::vector<int> &labels)
    {
        std::vector<std::future<void>> futures;
#pragma omp parallel for // Parallelize the loop across trees
        for (int i = 0; i < n_trees; ++i)
        {
            futures.push_back(std::async(std::launch::async, [this, &data, &labels, i]()
                                         {
            std::random_device rd;
            std::mt19937 g(rd());
            std::vector<std::vector<double>> bootstrapData;
            std::vector<int> bootstrapLabels;

            for (size_t j = 0; j < data.size(); ++j) {
                int index = std::uniform_int_distribution<>(0, data.size() - 1)(g);
                bootstrapData.push_back(data[index]);
                bootstrapLabels.push_back(labels[index]);
            }

            std::vector<int> featureIndices(data[0].size());
            std::iota(featureIndices.begin(), featureIndices.end(), 0);
            std::shuffle(featureIndices.begin(), featureIndices.end(), g);
            featureIndices.resize(std::sqrt(featureIndices.size()));

            trees[i].train(bootstrapData, bootstrapLabels, featureIndices); }));
        }

        for (auto &future : futures)
        {
#pragma omp future
            future.wait();
        }
    }

    int predict(const std::vector<double> &input)
    {
        std::unordered_map<int, int> classVotes;
        // #pragma omp parallel for
        for (auto &tree : trees)
        {
            int prediction = tree.predict(input);
            classVotes[prediction]++;
        }

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
// g++ -pg -fopenmp  -std=c++17 -o RF_ptr RandomForest_ptr.cpp -lpthread -O3 -march=native && ./RF_ptr