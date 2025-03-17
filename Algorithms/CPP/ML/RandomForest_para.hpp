#ifndef RANDOMFOREST_PARA_HPP
#define RANDOMFOREST_PARA_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <limits>
#include <random>
#include <memory>
#include <thread>
#include <omp.h>

// Node structure for decision tree
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
        // Serial loop to ensure thread-safety on the unordered_map
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

    // Check if all labels in the vector are identical (pure node)
    bool isPure(const std::vector<int>& labels) {
        return std::all_of(labels.begin(), labels.end(), [&](int label) { return label == labels[0]; });
    }

    // Recursively build the decision tree with improved synchronization
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
                                                   [](const std::pair<int,int>& a, const std::pair<int,int>& b) {
                                                       return a.second < b.second;
                                                   })->first;
            node->label = mostCommonLabel;
            return node;
        }

        int bestFeatureIndex = -1;
        double bestSplitValue = 0;
        double bestImpurity = std::numeric_limits<double>::max();

        // Parallel region: each thread maintains its own best candidate for the split
        #pragma omp parallel
        {
            int localBestFeatureIndex = -1;
            double localBestSplitValue = 0;
            double localBestImpurity = std::numeric_limits<double>::max();

            #pragma omp for nowait
            for (size_t i = 0; i < featureIndices.size(); i++) {
                int index = featureIndices[i];
                std::vector<double> featureValues;
                for (const auto &row : data) {
                    featureValues.push_back(row[index]);
                }
                std::sort(featureValues.begin(), featureValues.end());
                double splitValue = featureValues[featureValues.size() / 2];

                std::vector<int> leftLabels, rightLabels;
                for (size_t j = 0; j < data.size(); ++j) {
                    if (data[j][index] < splitValue) {
                        leftLabels.push_back(labels[j]);
                    } else {
                        rightLabels.push_back(labels[j]);
                    }
                }
                // Skip if either side is empty
                if (leftLabels.empty() || rightLabels.empty())
                    continue;

                double impurity = (calculateGini(leftLabels) * leftLabels.size() +
                                    calculateGini(rightLabels) * rightLabels.size()) / data.size();

                if (impurity < localBestImpurity) {
                    localBestImpurity = impurity;
                    localBestFeatureIndex = index;
                    localBestSplitValue = splitValue;
                }
            }
            // Critical section to update the global best candidate
            #pragma omp critical
            {
                if (localBestImpurity < bestImpurity) {
                    bestImpurity = localBestImpurity;
                    bestFeatureIndex = localBestFeatureIndex;
                    bestSplitValue = localBestSplitValue;
                }
            }
        } // End parallel region

        // If no valid split was found, return a leaf node
        if (bestFeatureIndex == -1) {
            auto node = std::make_unique<DecisionTreeNode>();
            std::unordered_map<int, int> labelCounts;
            for (int label : labels) {
                labelCounts[label]++;
            }
            int mostCommonLabel = std::max_element(labelCounts.begin(), labelCounts.end(),
                                                   [](const std::pair<int,int>& a, const std::pair<int,int>& b) {
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

    // Train the tree with an option to specify max depth
    void train(const std::vector<std::vector<double>> &data,
               const std::vector<int> &labels,
               const std::vector<int> &featureIndices,
               int maxDepth = 10) {
        root = buildTree(data, labels, featureIndices, 0, maxDepth);
    }

    // Predict the class for a given input vector
    int predict(const std::vector<double> &input) const {
        DecisionTreeNode *node = root.get();
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
    int maxDepth; // Maximum depth for individual trees

public:
    RandomForest(int n_trees, int maxDepth = 10)
        : n_trees(n_trees), maxDepth(maxDepth) {
        trees.resize(n_trees);
    }

    // Train the random forest using an OpenMP parallel for loop.
    void train(const std::vector<std::vector<double>>& data, const std::vector<int>& labels) {
        #pragma omp parallel for
        for (int i = 0; i < n_trees; i++) {
            // Each thread gets its own random generator
            std::random_device rd;
            std::mt19937 g(rd());

            // Create bootstrap sample
            std::vector<std::vector<double>> bootstrapData;
            std::vector<int> bootstrapLabels;
            bootstrapData.reserve(data.size());
            bootstrapLabels.reserve(labels.size());
            for (size_t j = 0; j < data.size(); j++) {
                int index = std::uniform_int_distribution<>(0, data.size() - 1)(g);
                bootstrapData.push_back(data[index]);
                bootstrapLabels.push_back(labels[index]);
            }

            // Random subset of features
            std::vector<int> featureIndices(data[0].size());
            std::iota(featureIndices.begin(), featureIndices.end(), 0);
            std::shuffle(featureIndices.begin(), featureIndices.end(), g);
            int subsetSize = std::max(1, static_cast<int>(std::sqrt(featureIndices.size())));
            featureIndices.resize(subsetSize);

            trees[i].train(bootstrapData, bootstrapLabels, featureIndices, maxDepth);
        }
    }

    // Predict the class by aggregating predictions from all trees
    int predict(const std::vector<double> &input) const {
        std::vector<int> predictions(trees.size());
        #pragma omp parallel for
        for (size_t i = 0; i < trees.size(); i++) {
            predictions[i] = trees[i].predict(input);
        }

        // Aggregate votes in a serial loop
        std::unordered_map<int, int> classVotes;
        for (int pred : predictions) {
            classVotes[pred]++;
        }

        int maxVotes = 0;
        int predictedClass = -1;
        for (const auto &vote : classVotes) {
            if (vote.second > maxVotes) {
                maxVotes = vote.second;
                predictedClass = vote.first;
            }
        }
        return predictedClass;
    }
};

#endif // RANDOMFOREST_PARA_HPP
