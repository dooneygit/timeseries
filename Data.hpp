#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "TimeSeries.hpp"
#include "CountryData.hpp"
#include "TreeNode.hpp"

class Data {
private:
    struct Relationship {
        std::string seriesCode;
        double threshold;
        std::string relation;
        bool operator==(const Relationship& other) const {
            return seriesCode == other.seriesCode && threshold == other.threshold && relation == other.relation;
        }
    };

    TreeNode* root = nullptr;
    CountryData countries[512];
    int numOfCountries = 0;
    std::string currSeriesCode;
    std::unordered_map<std::string, std::vector<std::string>> graphAdj;
    std::map<std::pair<std::string,std::string>, std::vector<Relationship>> edgeData;
    TreeNode* recursiveBuild(std::vector<std::string>& validCountries, double minMean, double maxMean, const std::unordered_map<std::string, double>& meanMap);
    void recursiveFind(TreeNode* node, double mean, const std::string& operation, bool& first, const std::unordered_map<std::string, double>& meanMap);
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
    void insertHelper(const std::string& country_code, const std::vector<std::string>& lines);
    TreeNode* buildTempTree(const std::string& series_code, std::unordered_map<std::string, double>& meanMap, std::unordered_map<std::string, std::string>& nameToCode);
    void collectCountries(TreeNode* node, double threshold, const std::string& relation, const std::unordered_map<std::string, double>& meanMap, std::vector<std::string>& result);
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
    void initialize();
    void update_edges(const std::string& series_code, double threshold, const std::string& relation);
    void adjacent(const std::string& country_code);
    void path(const std::string& code1, const std::string& code2);
    void relationships(const std::string& code1, const std::string& code2);
};