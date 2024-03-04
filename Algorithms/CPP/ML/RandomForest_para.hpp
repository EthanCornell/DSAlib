#ifndef RANDOMFOREST_PARA_HPP
#define RANDOMFOREST_PARA_HPP

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

    void train(const std::vector<std::vector<double>>& data, const std::vector<int>& labels) {
        std::vector<std::future<void>> futures;
        std::random_device rd;
        std::mt19937 g(rd());

        #pragma omp parallel for // Parallelize the loop across trees
        for (auto& tree : trees) {
            futures.push_back(std::async(std::launch::async, [&tree, &data, &labels, &g]() {
                std::vector<std::vector<double>> bootstrapData;
                std::vector<int> bootstrapLabels;

                for (size_t i = 0; i < data.size(); ++i) {
                    int index = std::uniform_int_distribution<>(0, data.size() - 1)(g);
                    bootstrapData.push_back(data[index]);
                    bootstrapLabels.push_back(labels[index]);
                }

                std::vector<int> featureIndices(data[0].size());
                std::iota(featureIndices.begin(), featureIndices.end(), 0);
                std::shuffle(featureIndices.begin(), featureIndices.end(), g);
                featureIndices.resize(std::sqrt(featureIndices.size()));

                tree.train(bootstrapData, bootstrapLabels, featureIndices);
            }));
        }

        // Wait for all futures to complete
        for (auto& future : futures) {
            future.wait();
        }
    }

    

    int predict(const std::vector<double> &input)
    {
        std::unordered_map<int, int> classVotes;
        #pragma omp parallel for
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


#endif // RANDOMFOREST_HPP