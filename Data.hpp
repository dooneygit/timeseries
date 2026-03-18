#pragma once
#include <string>
#include "TimeSeries.hpp"
#include "CountryData.hpp"
#include "TreeNode.hpp"

class Data {
private:
    TreeNode* root = nullptr;
    CountryData countries[512];
    int numOfCountries = 0;
    std::string currSeriesCode;
    TreeNode* recursiveBuild(std::string validCountries[], double minMean, double maxMean, int numOfValid, std::string series_code);
    void recursiveFind(TreeNode* node, double mean, std::string operation, bool& first);
    bool recursiveDelete(TreeNode* node, std::string country_name);
    void recursiveTrace(TreeNode* node, std::string country_name);
    bool hasCountry(TreeNode* node, std::string country_name);
    void clearTree(TreeNode* node);
public:
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
};