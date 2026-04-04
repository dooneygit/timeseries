#include "TreeNode.hpp"

TreeNode::TreeNode(double leftBound, double rightBound) : 
    min(leftBound), max(rightBound), left(nullptr), right(nullptr), numOfCountries(0), capacity(2) {
        countries = new std::string[capacity];
    }

TreeNode::~TreeNode() {
    delete left;
    delete right;
    delete[] countries;
}

void TreeNode::resize(int newCapacity) {
    std::string* newArray = new std::string[newCapacity];

    for(int i{0}; i < numOfCountries; i++) {
        newArray[i] = countries[i];
    }

    delete[] countries;
    countries = newArray;
    capacity = newCapacity;
}