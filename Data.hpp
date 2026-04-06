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
    TreeNode* recursiveBuild(std::vector<std::string>& validCountries, double minMean, double maxMean, const std::string& series_code);
    void recursiveFind(TreeNode* node, double mean, const std::string& operation, bool& first);
    bool recursiveDelete(TreeNode* node, const std::string& country_name);
    void recursiveTrace(TreeNode* node, const std::string& country_name);
    bool hasCountry(TreeNode* node, const std::string& country_name);
    void clearTree(TreeNode* node);
    int codeToInt(const std::string& country_code);
    int state[512]; // occupied = 1, empty = 0, previous occupied = -1
    int primaryHash(int key);
    int secondaryHash(int key);
    int hash(int key, int i);
    int search(const std::string& country_code, bool forInsertion, bool shouldPrint);
    void insertHelper(const std::string& country_code);
public:
    Data();
    ~Data();
    void load();
    void list(const std::string& country_name);
    void country_min(const std::string& country_code);
    void range(const std::string& series_code);
    void build(const std::string& series_code);
    void find(double mean, const std::string& operation);
    void deleteCountry(const std::string& country_name);
    void limits(const std::string& condition);
    void trace(const std::string& country_name);
    void lookup(const std::string& country_code);
    void remove(const std::string& country_code);
    void insert(const std::string& country_code);
    void clean();
};