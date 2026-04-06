#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_set>
#include "Data.hpp"
#include "TimeSeries.hpp"
#include "CountryData.hpp"

Data::Data() : root(nullptr), currSeriesCode(""), numOfCountries(0) {
    for(int i = 0; i < 512; i++) {
        state[i] = 0;
    }
}

Data::~Data() {
    clearTree(root);
    root = nullptr;
}

//hashing
int Data::codeToInt(const std::string& country_code) {
    if(country_code.size() < 3) {
        return -1;
    }

    int result = 0;

    for(int i{0}; i < 3; i++) {
        result = result * 26 + (country_code[i] - 'A');
    }

    return result;
}

int Data::primaryHash(int key) {
    return key % 512;
}

int Data::secondaryHash(int key) {
    int step = (key / 512) % 512;

    if(step % 2 == 0) {
        step++;
    }

    return step;
}

int Data::hash(int key, int i) {
    return (primaryHash(key) + i * secondaryHash(key)) % 512;
}

int Data::search(const std::string& country_code, bool forInsertion, bool shouldPrint) {
    int key = codeToInt(country_code); //turn country code into integer

    if(key < 0) { //invalid country code
        if(shouldPrint) {
            std::cout << "failure" << std::endl;
        }
        return -1;
    }

    int firstDeleted = -1;

    for(int i{0}; i < 512; i++) {
        int index = hash(key, i); //compute probe

        if(state[index] == 1) {
            if(countries[index].getCountryCode() == country_code) { //country already stored
                if(shouldPrint) {
                    std::cout << "index " << index << " searches " << i + 1 << std::endl; //print index and hash steps
                }
                return index; //return index
            }
        }
        else if(state[index] == -1) {
            if(forInsertion && firstDeleted == -1) {
                firstDeleted = index; //save first deleted slot for reuse
            }
        }
        else if(state[index] == 0) {
            if(forInsertion) {
                if(firstDeleted != -1) {
                    if(shouldPrint) {
                        std::cout << "index " << firstDeleted << " searches " << i + 1 << std::endl; //reuse first deleted slot
                    }
                    return firstDeleted;
                }

                if(shouldPrint) {
                    std::cout << "index " << index << " searches " << i + 1 << std::endl; //insert into empty slot
                }
                return index;
            }

            if(shouldPrint) {
                std::cout << "failure" << std::endl; //key was never found
            }
            return -1;
        }
    }

    if(forInsertion && firstDeleted != -1) {
        if(shouldPrint) {
            std::cout << "index " << firstDeleted << " searches 512" << std::endl;
        }
        return firstDeleted;
    }

    if(shouldPrint) {
        std::cout << "failure" << std::endl;
    }

    return -1;
}

//functions
void Data::load() {
    clearTree(root);
    root = nullptr;
    currSeriesCode = "";

    for(int i{0}; i < 512; i++) {
        countries[i].clear();
        state[i] = 0;
    }
    numOfCountries = 0;

    std::ifstream inputFile("lab2_multidata.csv");
    std::string line;

    std::getline(inputFile, line); // skip header

    while(std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string name, code;

        std::getline(ss, name, ',');
        std::getline(ss, code, ',');

        int countryIndex = search(code, true, false);

        if(countryIndex == -1) {
            continue;
        }

        if(state[countryIndex] != 1) {
            countries[countryIndex].clear();
            countries[countryIndex].setCountryName(name);
            countries[countryIndex].setCountryCode(code);
            state[countryIndex] = 1;
            numOfCountries++;
        }

        countries[countryIndex].addSeriesFromRow(line);
    }

    std::cout << "success" << std::endl;
}

void Data::list(const std::string& country_name) { 
    for(int i{0}; i < 512; i++) { //go through all possible hash table slots
        if(state[i] == 1 && countries[i].getCountryName() == country_name) { //if target country is found, print all series
            countries[i].list();
            return;
        }
    }
}

void Data::country_min(const std::string& country_code) { //search for the country
    int countryIndex = search(country_code, false, false);
    if(countryIndex == -1 || state[countryIndex] != 1) {
        std::cout << "failure" << std::endl;
        return;
    }

    countries[countryIndex].smallest(); //print the series code with the smallest mean
}

void Data::range(const std::string& series_code) { 
    double minMean = -1.0; //tracks smallest and largest so far
    double maxMean = -1.0;

    for(int i{0}; i < 512; i++) { //check series for every occupied country slot
        if(state[i] != 1) {
            continue;
        }

        int seriesIndex = countries[i].findSeriesCode(series_code);
        if(seriesIndex == -1) {
            continue;
        }

        double mean = countries[i].series[seriesIndex]->meanValue(); //compute mean of matching series
        
        if(mean <= 0) { //ignore invalid
            continue;
        }

        if(minMean <= 0) { //first valid mean initializes min and max mean so far
            minMean = mean;
            maxMean = mean;
        }

        if(minMean > mean) { //update min
            minMean = mean;
        }
        else if(mean > maxMean) { //update max
            maxMean = mean;
        }
    }

    std::cout << minMean << " " << maxMean << std::endl;
}

void Data::build(const std::string& series_code) {
    clearTree(root);
    root = nullptr;
    currSeriesCode = series_code;

    std::vector<std::string> validCountries;
    std::unordered_map<std::string, double> meanMap;
    double minMean = -1.0;
    double maxMean = -1.0;

    for(int i{0}; i < 512; i++) {
        if(state[i] != 1) {
            continue;
        }

        int seriesIndex = countries[i].findSeriesCode(series_code);
        if(seriesIndex == -1) {
            continue;
        }

        double mean = countries[i].series[seriesIndex]->meanValue();

        if(mean <= 0) {
            continue;
        }

        std::string name = countries[i].getCountryName();
        validCountries.push_back(name);
        meanMap[name] = mean;

        if(minMean == -1.0) {
            minMean = mean;
            maxMean = mean;
        }

        if(minMean > mean) {
            minMean = mean;
        }
        else if(maxMean < mean) {
            maxMean = mean;
        }
    }

    if(validCountries.size() > 0) {
        root = recursiveBuild(validCountries, minMean, maxMean, meanMap);
    }
    std::cout << "success" << std::endl;
}

TreeNode* Data::recursiveBuild(std::vector<std::string>& validCountries, double minMean, double maxMean, const std::unordered_map<std::string, double>& meanMap) {
    int numOfValid = validCountries.size();
    TreeNode* node = new TreeNode(minMean, maxMean);

    for(int i{0}; i < numOfValid; i++) {
        if(node->numOfCountries == node->capacity) {
            node->resize(node->capacity * 2);
        }

        node->countries[node->numOfCountries] = validCountries[i];
        node->numOfCountries++;
    }

    if(numOfValid == 1) {
        return node;
    }

    double firstMean = -1.0;
    bool close = true;

    for(int i{0}; i < numOfValid; i++) {
        double mean = meanMap.at(validCountries[i]);

        if(i == 0) {
            firstMean = mean;
        }
        else {
            double diff = mean - firstMean;
            if(diff < 0) {
                diff = -diff;
            }
            if(diff > 1e-3) {
                close = false;
                break;
            }
        }
    }

    if(close) {
        return node;
    }

    double mid = (minMean + maxMean)/2.0;
    std::vector<std::string> leftCountries;
    std::vector<std::string> rightCountries;

    for(int i{0}; i < numOfValid; i++) {
        double mean = meanMap.at(validCountries[i]);

        if(mean < mid) {
            leftCountries.push_back(validCountries[i]);
        }
        else {
            rightCountries.push_back(validCountries[i]);
        }
    }

    if(leftCountries.empty() || rightCountries.empty()) {
        return node;
    }

    node->left = recursiveBuild(leftCountries, minMean, mid, meanMap);
    node->right = recursiveBuild(rightCountries, mid, maxMean, meanMap);
    return node;
}

void Data::find(double mean, const std::string& operation) {
    if(root == nullptr) {
        std::cout << "failure" << std::endl;
        return;
    }

    std::unordered_map<std::string, double> meanMap;

    for(int i{0}; i < 512; i++) {
        if(state[i] != 1) {
            continue;
        }

        int seriesIndex = countries[i].findSeriesCode(currSeriesCode);
        if(seriesIndex == -1) {
            continue;
        }

        double m = countries[i].series[seriesIndex]->meanValue();

        if(m > 0) {
            meanMap[countries[i].getCountryName()] = m;
        }
    }

    bool first = true;

    recursiveFind(root, mean, operation, first, meanMap);
    std::cout << std::endl;
}

void Data::recursiveFind(TreeNode* node, double mean, const std::string& operation, bool& first, const std::unordered_map<std::string, double>& meanMap) {
    if(node == nullptr) {
        return;
    }

    if(operation == "less" && node->min >= mean) {
        return;
    }

    if(operation == "greater" && node->max <= mean) {
        return;
    }

    if(operation == "equal" && (node->max < mean - 1e-3 || node->min > mean + 1e-3)) {
        return;
    }

    if(node->left == nullptr && node->right == nullptr) {
        for(int i{0}; i < node->numOfCountries; i++) {
            auto it = meanMap.find(node->countries[i]);
            if(it == meanMap.end()) {
                continue;
            }

            double currMean = it->second;

            if(operation == "less" && currMean < mean) {
                if(!first) {
                    std::cout << " ";
                }
                std::cout << node->countries[i];
                first = false;
            }
            else if(operation == "greater" && currMean > mean) {
                if(!first) {
                    std::cout << " ";
                }
                std::cout << node->countries[i];
                first = false;
            }
            else if(operation == "equal") {
                double diff = currMean - mean;
                if(diff < 0) {
                    diff = -diff;
                }
                if(diff <= 1e-3) {
                    if(!first) {
                        std::cout << " ";
                    }
                    std::cout << node->countries[i];
                    first = false;
                }
            }
        }

        return;
    }

    recursiveFind(node->left, mean, operation, first, meanMap);
    recursiveFind(node->right, mean, operation, first, meanMap);
}

void Data::deleteCountry(const std::string& country_name) {
    if(root == nullptr) { //if no tree exists
        std::cout << "failure" << std::endl;
        return;
    }

    int index = -1;
    for(int i{0}; i < 512; i++) {
        if(state[i] == 1 && countries[i].getCountryName() == country_name) {
            index = i;
            break;
        }
    }

    bool found = recursiveDelete(root, country_name);

    if(root != nullptr && root->left == nullptr && root->right == nullptr && root->numOfCountries == 0) { 
        delete root;
        root = nullptr;
    }

    if(found) {
        if(index != -1) {
            countries[index].clear();
            state[index] = -1;
            numOfCountries--;
        }
        std::cout << "success" << std::endl;
    }
    else {
        std::cout << "failure" << std::endl;
    }
}

bool Data::recursiveDelete(TreeNode* node, const std::string& country_name) {
    if(node == nullptr) {
        return false;
    }

    bool foundHere = false; //current node contains country

    for(int i = 0; i < node->numOfCountries; i++) { //shift entries left
        if(node->countries[i] == country_name) {
            foundHere = true;

            for(int j = i; j < node->numOfCountries - 1; j++) {
                node->countries[j] = node->countries[j + 1];
            }

            node->numOfCountries--;
            break;
        }
    }

    if(!foundHere) {
        return false;
    }

    recursiveDelete(node->left, country_name);
    recursiveDelete(node->right, country_name);

    if(node->left != nullptr) {
        if(node->left->left == nullptr && node->left->right == nullptr && node->left->numOfCountries == 0) {
            delete node->left;
            node->left = nullptr;
        }
    }

    if(node->right != nullptr) {
        if(node->right->left == nullptr && node->right->right == nullptr && node->right->numOfCountries == 0) {
            delete node->right;
            node->right = nullptr;
        }
    }

    return true;
}

void Data::limits(const std::string& condition) {
    if(root == nullptr) {
        std::cout << "failure" << std::endl; 
        return;
    }

    TreeNode* temp = root; 

    //go all the way left for lowest and all the way right for highest
    if(condition == "lowest") { 
        while(temp->left != nullptr) {
            temp = temp->left;
        }
    }
    else {
        while(temp->right != nullptr) {
            temp = temp->right;
        }
    }

    for(int i = 0; i < temp->numOfCountries; i++) {
        std::cout << temp->countries[i];

        if(i != temp->numOfCountries - 1) {
            std::cout << " ";
        }
    }

    std::cout << std::endl;
}

void Data::trace(const std::string& country_name) {
    if(root == nullptr) {
        std::cout << "failure" << std::endl;
        return;
    }

    if(!hasCountry(root, country_name)) {
        std::cout << "failure" << std::endl;
        return;
    }

    recursiveTrace(root, country_name);
    std::cout << std::endl;
}

void Data::recursiveTrace(TreeNode* node, const std::string& country_name) {
    if(node == nullptr) { 
        return;
    }

    std::cout << node->min << " " << node->max;

    if(node->left != nullptr && hasCountry(node->left, country_name)) { //if country is in left subtree, continue left
        std::cout << " ";
        recursiveTrace(node->left, country_name);
    }
    else if(node->right != nullptr && hasCountry(node->right, country_name)) { //if country is in right subtree, continue right
        std::cout << " ";
        recursiveTrace(node->right, country_name);
    }
}

bool Data::hasCountry(TreeNode* node, const std::string& country_name) {
    if(node == nullptr) {
        return false;
    }

    for(int i = 0; i < node->numOfCountries; i++) {
        if(node->countries[i] == country_name) {
            return true;
        }
    }

    return false;
}

void Data::clearTree(TreeNode* node) {
    if(node == nullptr) {
        return;
    }

    clearTree(node->left);
    clearTree(node->right);
    delete node;
}

void Data::lookup(const std::string& country_code) {
    search(country_code, false, true); //search for country and print results from search()
}

void Data::remove(const std::string& country_code) {
    int countryIndex = search(country_code, false, false); //search for country

    if(countryIndex != -1 && state[countryIndex] == 1) {
        std::string country_name = countries[countryIndex].getCountryName(); //save country name

        countries[countryIndex].clear(); //erase country data
        state[countryIndex] = -1; //mark slot as previously occupied
        numOfCountries--;
    
        if(root != nullptr) {
            bool found = recursiveDelete(root, country_name); //remove country from tree
    
            if(root != nullptr && root->left == nullptr &&
               root->right == nullptr && root->numOfCountries == 0) {
                delete root; //clean up tree
                root = nullptr;
            }
        }
    
        std::cout << "success" << std::endl;
    }
    else {
        std::cout << "failure" << std::endl;
    }
}

void Data::insert(const std::string& country_code) {
    int countryIndex = search(country_code, true, false); //find where country should be inserted

    if(countryIndex == -1 || state[countryIndex] == 1) {
        std::cout << "failure" << std::endl; //fail if table is full
        return;
    }

    std::ifstream inputFile("lab2_multidata.csv");
    std::string line;
    bool found = false;
    std::string name, code;

    std::getline(inputFile, line); // skip header

    while(std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string currName, currCode;

        std::getline(ss, currName, ',');
        std::getline(ss, currCode, ',');

        if(currCode != country_code) {
            continue; //ignore rows for other countries
        }

        if(!found) {
            countries[countryIndex].clear(); //reset before loading country's rows
            countries[countryIndex].setCountryName(currName);
            countries[countryIndex].setCountryCode(currCode);
            state[countryIndex] = 1;
            numOfCountries++;
            found = true;
        }

        countries[countryIndex].addSeriesFromRow(line);
    }

    if(found) {
        if(root != nullptr) {
            clearTree(root);
            root = nullptr;
            currSeriesCode = "";
        }
        std::cout << "success" << std::endl;
    }
    else {
        std::cout << "failure" << std::endl; //code not found in file
    }
}

void Data::insertHelper(const std::string& country_code, const std::vector<std::string>& lines) {
    int countryIndex = search(country_code, true, false);

    if(countryIndex == -1 || state[countryIndex] == 1) {
        std::cout << "failure" << std::endl;
        return;
    }

    bool found = false;
    int numLines = (int)lines.size();

    for(int i{0}; i < numLines; i++) {
        std::stringstream ss(lines[i]);
        std::string currName, currCode;

        std::getline(ss, currName, ',');
        std::getline(ss, currCode, ',');

        if(currCode != country_code) {
            continue;
        }

        if(!found) {
            countries[countryIndex].clear();
            countries[countryIndex].setCountryName(currName);
            countries[countryIndex].setCountryCode(currCode);
            state[countryIndex] = 1;
            numOfCountries++;
            found = true;
        }

        countries[countryIndex].addSeriesFromRow(lines[i]);
    }
}

void Data::clean() {
    std::string temp[512];
    int count = 0;

    for(int i{0}; i < 512; i++) {
        if(state[i] == 1) {
            temp[count] = countries[i].getCountryCode(); //collect only stored countries
            count++;
        }
    }

    for(int i{1}; i < count; i++) {
        std::string key = temp[i]; //insert current code into sorted position
        int j = i - 1;

        while(j >= 0 && temp[j] > key) { //shift larger codes right
            temp[j + 1] = temp[j];
            j--;
        }

        temp[j + 1] = key;
    }

    for(int i{0}; i < 512; i++) {
        countries[i].clear();
        state[i] = 0;
    }

    numOfCountries = 0;

    if(root != nullptr) {
        clearTree(root);
        root = nullptr;
        currSeriesCode = "";
    }

    std::ifstream inputFile("lab2_multidata.csv");
    std::vector<std::string> lines;
    std::string line;

    std::getline(inputFile, line);

    while(std::getline(inputFile, line)) {
        lines.push_back(line);
    }

    for(int i{0}; i < count; i++) {
        insertHelper(temp[i], lines);
    }

    std::cout << "success" << std::endl;
}

void Data::initialize() {
    edgeData.clear();
    graphAdj.clear();

    for(int i{0}; i < 512; i++) {
        if(state[i] == 1) {
            graphAdj[countries[i].getCountryCode()] = {}; //each country starts as a disconnected node
        }
    }

    std::cout << "success" << std::endl;
}

TreeNode* Data::buildTempTree(const std::string& series_code, std::unordered_map<std::string, double>& meanMap, std::unordered_map<std::string, std::string>& nameToCode) {
    std::vector<std::string> validCountries;
    double minMean = -1.0;
    double maxMean = -1.0;

    for(int i{0}; i < 512; i++) {
        if(state[i] != 1) {
            continue;
        }

        int seriesIndex = countries[i].findSeriesCode(series_code);
        if(seriesIndex == -1) {
            continue;
        }

        double mean = countries[i].series[seriesIndex]->meanValue();
        if(mean <= 0) {
            continue;
        }

        std::string name = countries[i].getCountryName();
        validCountries.push_back(name);
        meanMap[name] = mean;
        nameToCode[name] = countries[i].getCountryCode(); //need the code later to create edges, the tree only stores names

        if(minMean == -1.0) {
            minMean = mean;
            maxMean = mean;
        }

        if(mean < minMean) {
            minMean = mean;
        }
        else if(mean > maxMean) {
            maxMean = mean;
        }
    }

    if(validCountries.empty()) {
        return nullptr;
    }

    return recursiveBuild(validCountries, minMean, maxMean, meanMap); //build without overwriting the existing member tree
}

void Data::collectCountries(TreeNode* node, double threshold, const std::string& relation, const std::unordered_map<std::string, double>& meanMap, std::vector<std::string>& result) {
    if(node == nullptr) {
        return;
    }

    if(relation == "less" && node->min >= threshold) { //no country in this subtree can have a mean below the threshold
        return;
    }
    if(relation == "greater" && node->max <= threshold) { //no country in this subtree can have a mean above the threshold
        return;
    }
    if(relation == "equal" && (node->max < threshold - 1e-3 || node->min > threshold + 1e-3)) { //threshold is out of range for anything in this subtree
        return;
    }

    if(node->left == nullptr && node->right == nullptr) { //countries are only stored at leaf nodes
        for(int i{0}; i < node->numOfCountries; i++) {
            auto it = meanMap.find(node->countries[i]);
            if(it == meanMap.end()) {
                continue;
            }

            double mean = it->second;

            if(relation == "less" && mean < threshold) {
                result.push_back(node->countries[i]); //country satisfies the relation
            }
            else if(relation == "greater" && mean > threshold) {
                result.push_back(node->countries[i]); //country satisfies the relation
            }
            else if(relation == "equal") {
                double diff = mean - threshold;
                if(diff < 0) diff = -diff;
                if(diff <= 1e-3) {
                    result.push_back(node->countries[i]); //country satisfies the relation
                }
            }
        }
        return;
    }

    collectCountries(node->left, threshold, relation, meanMap, result);
    collectCountries(node->right, threshold, relation, meanMap, result);
}

void Data::update_edges(const std::string& series_code, double threshold, const std::string& relation) {
    std::unordered_map<std::string, double> meanMap;
    std::unordered_map<std::string, std::string> nameToCode;

    TreeNode* tempTree = buildTempTree(series_code, meanMap, nameToCode); //build a tree for this series to find countries matching the relation

    if(tempTree == nullptr) {
        std::cout << "failure" << std::endl;
        return;
    }

    std::vector<std::string> qualifyingNames;
    collectCountries(tempTree, threshold, relation, meanMap, qualifyingNames); //search the tree for countries that satisfy the relation

    clearTree(tempTree); //done searching, free the temporary tree

    if(qualifyingNames.size() < 2) {
        std::cout << "failure" << std::endl;
        return;
    }

    std::vector<std::string> qualifyingCodes;
    for(const std::string& name : qualifyingNames) {
        auto it = nameToCode.find(name);
        if(it != nameToCode.end()) {
            qualifyingCodes.push_back(it->second); //edges are keyed by country code, not name
        }
    }

    if(qualifyingCodes.size() < 2) {
        std::cout << "failure" << std::endl;
        return;
    }

    Relationship rel{series_code, threshold, relation};
    bool anyAdded = false;

    for(int i{0}; i < (int)qualifyingCodes.size(); i++) {
        for(int j = i + 1; j < (int)qualifyingCodes.size(); j++) {
            std::string lo = qualifyingCodes[i];
            std::string hi = qualifyingCodes[j];

            if(lo > hi) { //always store smaller code first so each pair has one unique key
                std::string temp = lo;
                lo = hi;
                hi = temp;
            }

            auto key = std::make_pair(lo, hi);
            auto it = edgeData.find(key);

            if(it == edgeData.end()) { //new edge, add it
                edgeData[key] = {rel};
                graphAdj[qualifyingCodes[i]].push_back(qualifyingCodes[j]);
                graphAdj[qualifyingCodes[j]].push_back(qualifyingCodes[i]); //undirected, add from both sides
                anyAdded = true;
            }
            else { //edge exists, check if this relationship is already there
                bool exists = false;
                for(const Relationship& r : it->second) {
                    if(r == rel) {
                        exists = true;
                        break;
                    }
                }

                if(!exists) { //spec says store only unique tuples per edge
                    it->second.push_back(rel);
                    anyAdded = true;
                }
            }
        }
    }

    if(anyAdded) {
        std::cout << "success" << std::endl;
    }
    else {
        std::cout << "failure" << std::endl;
    }
}

void Data::adjacent(const std::string& country_code) {
    auto it = graphAdj.find(country_code);

    if(it == graphAdj.end()) { //country is not in the graph
        std::cout << "failure" << std::endl;
        return;
    }

    if(it->second.empty()) { //country has no edges yet
        std::cout << "none" << std::endl;
        return;
    }

    bool first = true;
    for(const std::string& adjCode : it->second) {
        int idx = search(adjCode, false, false); //look up country data by code to get the name
        if(idx == -1 || state[idx] != 1) {
            continue;
        }

        if(!first) {
            std::cout << " ";
        }
        std::cout << countries[idx].getCountryName(); //output name not code
        first = false;
    }

    std::cout << std::endl;
}

void Data::path(const std::string& code1, const std::string& code2) {
    if(code1 == code2) { //same country is trivially connected
        std::cout << "true" << std::endl;
        return;
    }

    std::unordered_set<std::string> visited;
    std::queue<std::string> bfsQueue;

    bfsQueue.push(code1);
    visited.insert(code1);

    while(!bfsQueue.empty()) {
        std::string curr = bfsQueue.front();
        bfsQueue.pop();

        auto it = graphAdj.find(curr);
        if(it == graphAdj.end()) {
            continue;
        }

        for(const std::string& neighbor : it->second) {
            if(neighbor == code2) { //found a path to the destination
                std::cout << "true" << std::endl;
                return;
            }

            if(visited.find(neighbor) == visited.end()) { //only enqueue countries not yet visited
                visited.insert(neighbor);
                bfsQueue.push(neighbor);
            }
        }
    }

    std::cout << "false" << std::endl;
}

void Data::relationships(const std::string& code1, const std::string& code2) {
    std::string lo = code1;
    std::string hi = code2;

    if(lo > hi) { //match the key order used when the edge was stored
        std::swap(lo, hi);
    }

    auto it = edgeData.find(std::make_pair(lo, hi));

    if(it == edgeData.end() || it->second.empty()) { //no edge between these two countries
        std::cout << "none" << std::endl;
        return;
    }

    bool first = true;
    for(const Relationship& r : it->second) {
        if(!first) {
            std::cout << " ";
        }
        std::cout << "(" << r.seriesCode << " " << r.threshold << " " << r.relation << ")"; //format each tuple
        first = false;
    }

    std::cout << std::endl;
}
