#include <iostream>
#include <vector>
#include <memory>
#include <limits>
#include <cstdlib>
#include <climits>

#define MAX_LEVEL 6

// Node structure using smart pointers
struct Node {
    int value;
    std::vector<std::shared_ptr<Node>> forward;

    Node(int val, int level) : value(val), forward(level + 1) {}
};

// SkipList structure
class SkipList {
    int currentLevel;
    std::shared_ptr<Node> header;

public:
    SkipList() : currentLevel(0), header(std::make_shared<Node>(INT_MIN, MAX_LEVEL)) {}

    void insert(int value) {
        std::vector<std::shared_ptr<Node>> update(MAX_LEVEL + 1);
        std::shared_ptr<Node> current = header;

        for (int i = currentLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (!current || current->value != value) {
            int level = randomLevel();
            if (level > currentLevel) {
                for (int i = currentLevel + 1; i <= level; i++) {
                    update[i] = header;
                }
                currentLevel = level;
            }

            auto newNode = std::make_shared<Node>(value, level);
            for (int i = 0; i <= level; i++) {
                newNode->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = newNode;
            }
        }
    }

    bool search(int value) {
        std::shared_ptr<Node> current = header;
        for (int i = currentLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        return current && current->value == value;
    }

    void remove(int value) {
        std::vector<std::shared_ptr<Node>> update(MAX_LEVEL + 1);
        std::shared_ptr<Node> current = header;

        for (int i = currentLevel; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->value == value) {
            for (int i = 0; i <= currentLevel; i++) {
                if (update[i]->forward[i] != current) break;
                update[i]->forward[i] = current->forward[i];
            }

            while (currentLevel > 0 && !header->forward[currentLevel]) {
                currentLevel--;
            }
        }
    }

private:
    int randomLevel() {
        int level = 0;
        while (rand() % 2 && level < MAX_LEVEL) {
            level++;
        }
        return level;
    }
};

// Main function with test cases
int main() {
    SkipList sl;

    sl.insert(3);
    sl.insert(6);
    sl.insert(7);
    sl.insert(9);
    sl.insert(12);
    sl.insert(17);
    sl.insert(19);

    std::cout << "Search 9: " << (sl.search(9) ? "Found" : "Not Found") << std::endl;
    std::cout << "Search 15: " << (sl.search(15) ? "Found" : "Not Found") << std::endl;

    sl.remove(7);
    std::cout << "Search 7 after deletion: " << (sl.search(7) ? "Found" : "Not Found") << std::endl;

    return 0;
}
