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


// Each Node contains a data element and a unique_ptr to the next node.
// The push method creates a new node and places it on top of
// the stack.

// The pop method removes the top element from the stack and returns its value. It automatically deallocates the memory for the node thanks to the unique_ptr.
// The top method returns the value of the top element without removing it from the stack.
// The isEmpty method checks whether the stack is empty.
// This implementation ensures that each node is properly managed and deleted when no longer needed, thanks to the automatic memory management features of std::unique_ptr. This approach eliminates the need for explicit memory deallocation, thus reducing the risk of memory leaks.

#include <iostream>
#include <memory>

template<typename T>
class Stack {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;

        Node(T data) : data(data), next(nullptr) {}
    };

    std::unique_ptr<Node> topNode;

public:
    void push(T data) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>(data);
        newNode->next = std::move(topNode);
        topNode = std::move(newNode);
    }

    T pop() {
        if (isEmpty()) {
            throw std::out_of_range("Stack is empty");
        }
        T data = topNode->data;
        topNode = std::move(topNode->next);
        return data;
    }

    T top() const {
        if (isEmpty()) {
            throw std::out_of_range("Stack is empty");
        }
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }
};

int main() {
    Stack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);

    while (!stack.isEmpty()) {
        std::cout << stack.pop() << std::endl;
    }

    return 0;
}
