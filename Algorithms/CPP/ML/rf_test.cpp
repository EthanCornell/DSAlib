#include "randomforest_para.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <random>
#include <numeric>

// Helper function to print a vector
void printVector(const std::vector<double>& vec) {
    for (double v : vec)
        std::cout << v << " ";
    std::cout << "\n";
}

// Helper function to compute accuracy for a given dataset
double computeAccuracy(RandomForest &rf, const std::vector<std::vector<double>> &data, const std::vector<int> &labels) {
    int correct = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        int pred = rf.predict(data[i]);
        if (pred == labels[i])
            ++correct;
    }
    return static_cast<double>(correct) / data.size();
}

// Test 1: Simple dataset test
void testSimpleDataset() {
    std::cout << "Running testSimpleDataset...\n";
    // Two clearly separable clusters
    std::vector<std::vector<double>> data = {
        {1.0, 1.2},
        {1.1, 0.9},
        {0.9, 1.0},
        {10.0, 10.1},
        {10.2, 9.8},
        {9.9, 10.0}
    };
    std::vector<int> labels = {0, 0, 0, 1, 1, 1};

    RandomForest rf(10, 5);
    rf.train(data, labels);
    double accuracy = computeAccuracy(rf, data, labels);
    std::cout << "Simple dataset accuracy: " << accuracy << "\n";
    assert(accuracy >= 0.8); // Expect at least 80% accuracy on this simple dataset
}

// Test 2: Noise robustness test
void testNoiseRobustness() {
    std::cout << "Running testNoiseRobustness...\n";
    // Same basic dataset but add noise
    std::vector<std::vector<double>> data = {
        {1.0, 1.2},
        {1.1, 0.9},
        {0.9, 1.0},
        {10.0, 10.1},
        {10.2, 9.8},
        {9.9, 10.0}
    };
    std::vector<int> labels = {0, 0, 0, 1, 1, 1};

    // Add Gaussian noise to the dataset
    std::default_random_engine eng(42);
    std::normal_distribution<double> noise(0.0, 0.5);
    std::vector<std::vector<double>> noisyData = data;
    for (auto &row : noisyData) {
        for (auto &val : row) {
            val += noise(eng);
        }
    }

    RandomForest rf(10, 5);
    rf.train(noisyData, labels);
    double accuracy = computeAccuracy(rf, noisyData, labels);
    std::cout << "Noisy dataset accuracy: " << accuracy << "\n";
    assert(accuracy >= 0.7); // Expect slightly lower accuracy on noisy data
}

// Test 3: Single class test
void testSingleClass() {
    std::cout << "Running testSingleClass...\n";
    // All data points belong to the same class
    std::vector<std::vector<double>> data = {
        {1.0, 1.2},
        {1.1, 0.9},
        {0.9, 1.0},
        {1.0, 1.1}
    };
    std::vector<int> labels = {0, 0, 0, 0};

    RandomForest rf(5, 3);
    rf.train(data, labels);
    double accuracy = computeAccuracy(rf, data, labels);
    std::cout << "Single class dataset accuracy: " << accuracy << "\n";
    assert(accuracy == 1.0); // Should predict perfectly
}

// Test 4: Single sample test
void testSingleSample() {
    std::cout << "Running testSingleSample...\n";
    std::vector<std::vector<double>> data = {
        {2.5, 3.1}
    };
    std::vector<int> labels = {1};

    RandomForest rf(3, 2);
    rf.train(data, labels);
    int prediction = rf.predict(data[0]);
    std::cout << "Single sample predicted label: " << prediction << "\n";
    assert(prediction == 1);
}

// Test 5: Cross-validation test (k-fold)
void testCrossValidation() {
    std::cout << "Running testCrossValidation...\n";
    // Generate a larger synthetic dataset with two clusters
    const int n = 100;
    std::vector<std::vector<double>> data;
    std::vector<int> labels;
    std::default_random_engine eng(42);
    std::normal_distribution<double> dist0(1.0, 0.5);
    std::normal_distribution<double> dist1(10.0, 0.5);

    for (int i = 0; i < n; ++i) {
        std::vector<double> point;
        if (i < n / 2) {
            point.push_back(dist0(eng));
            point.push_back(dist0(eng));
            labels.push_back(0);
        } else {
            point.push_back(dist1(eng));
            point.push_back(dist1(eng));
            labels.push_back(1);
        }
        data.push_back(point);
    }

    // 5-fold cross-validation
    int k = 5;
    int foldSize = n / k;
    double totalAccuracy = 0;
    for (int fold = 0; fold < k; ++fold) {
        std::vector<std::vector<double>> trainData;
        std::vector<int> trainLabels;
        std::vector<std::vector<double>> testData;
        std::vector<int> testLabels;
        for (int i = 0; i < n; ++i) {
            if (i >= fold * foldSize && i < (fold + 1) * foldSize) {
                testData.push_back(data[i]);
                testLabels.push_back(labels[i]);
            } else {
                trainData.push_back(data[i]);
                trainLabels.push_back(labels[i]);
            }
        }
        RandomForest rf(10, 5);
        rf.train(trainData, trainLabels);
        double accuracy = computeAccuracy(rf, testData, testLabels);
        std::cout << "Fold " << fold << " accuracy: " << accuracy << "\n";
        totalAccuracy += accuracy;
    }
    double avgAccuracy = totalAccuracy / k;
    std::cout << "Average cross-validation accuracy: " << avgAccuracy << "\n";
    assert(avgAccuracy >= 0.8); // Expect reasonable performance on synthetic data
}

int main() {
    testSimpleDataset();
    testNoiseRobustness();
    testSingleClass();
    testSingleSample();
    testCrossValidation();

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
// g++ -o rf_test rf_test.cpp -std=c++17 -fopenmp