#pragma once
#include <string>
#include <vector>
#include "TimeSeries.hpp"
#include "CountryData.hpp"
#include "TreeNode.hpp"

class Data {
private:
    TreeNode* root = nullptr;
    CountryData countries[512];
    int numOfCountries = 0;
    std::string currSeriesCode;
    TreeNode* recursiveBuild(std::vector<std::string>& validCountries, double minMean, double maxMean, std::string series_code);
    void recursiveFind(TreeNode* node, double mean, std::string operation, bool& first);
    bool recursiveDelete(TreeNode* node, std::string country_name);
    void recursiveTrace(TreeNode* node, std::string country_name);
    bool hasCountry(TreeNode* node, std::string country_name);
    void clearTree(TreeNode* node);
    int codeToInt(std::string country_code);
    int state[512]; // occupied = 1, empty = 0, previous occupied = -1
    int primaryHash(int key);
    int secondaryHash(int key);
    int hash(int key, int i);
    int search(std::string country_code, bool forInsertion, bool shouldPrint);
    void insertHelper(std::string country_code);
public:
    Data();
    ~Data();
    void load();
    void list(const std::string country_name);
    void country_min(std::string country_code);
    void range(std::string series_code);
    void build(std::string series_code);
    void find(double mean, std::string operation);
    void deleteCountry(std::string country_name);
    void limits(std::string condition);
    void trace(std::string country_name);
    void lookup(std::string country_code);
    void remove(std::string country_code);
    void insert(std::string country_code);
    void clean();
};