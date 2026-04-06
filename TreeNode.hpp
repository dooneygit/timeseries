#pragma once
#include <string>

class TreeNode {
public:
    double min;
    double max;
    TreeNode* left;
    TreeNode* right;
    std::string* countries; 
    int numOfCountries;
    int capacity;

    TreeNode(double leftBound, double rightBound);
    ~TreeNode();
    TreeNode(const TreeNode&) = delete;
    TreeNode& operator=(const TreeNode&) = delete;
    void resize(int newCapacity);
};