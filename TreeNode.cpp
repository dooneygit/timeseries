#include "TreeNode.hpp"

TreeNode::TreeNode(double left, double right) : 
    left(left), right(right), numOfCountries(0), capacity(2), left(nullptr), right(nullptr) {
        countries = new std::string[capacity];
    }

TreeNode::~TreeNode() {
    delete[] countries;
}

void TreeNode::resize(int newCapacity) {
    std::string* newArray = new std::string[newCapacity];

    for(int i{0}; i < numOfCountries; i++) {
        newArray[i] = countries[i];
    }

    delete countries[];
    countries = newArray;
    capacity = newCapacity;
}

void TreeNode::addCountry(std::string countryName) {
    if(numCountries == capacity) {
        resize(capacity * 2);
    }

    countries[numOfCountries] = countryName;
    numOfCountries++;
}

