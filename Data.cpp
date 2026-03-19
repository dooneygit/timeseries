#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Data.hpp"
#include "TimeSeries.hpp"
#include "CountryData.hpp"

Data::~Data() {
    clearTree(root);
    root = nullptr;
}

//hashing
int Data::codeToInt(std::string country_code) {
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

int Data::search(std::string country_code, bool forInsertion, bool shouldPrint) {
    int key = codeToInt(country_code);
    int firstDeleted = -1;

    for(int i{0}; i < 512; i++) {
        int index = hash(key, i);

        if(state[index] == 1) {
            if(countries[index].getCountryCode() == country_code) {
                if(shouldPrint) {
                    std::cout << "index " << index << " searches " << i + 1 << std::endl;
                }
                return index;
            }
        }
        else if(state[index] == -1) {
            if(forInsertion && firstDeleted == -1) {
                firstDeleted = index;
            }
        }
        else if(state[index] == 0) {
            if(forInsertion) {
                if(firstDeleted != -1) {
                    if(shouldPrint) {
                        std::cout << "index " << firstDeleted << " searches " << i + 1 << std::endl;
                    }
                    return firstDeleted;
                }

                if(shouldPrint) {
                    std::cout << "index " << index << " searches " << i + 1 << std::endl;
                }
                return index;
            }

            if(shouldPrint) {
                std::cout << "failure" << std::endl;
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

void Data::list(const std::string country_name) { 
    for(int i{0}; i < 512; i++) { //go through all possible hash table slots
        if(state[i] == 1 && countries[i].getCountryName() == country_name) { //if target country is found, print all series
            countries[i].list();
            return;
        }
    }
}

void Data::country_min(std::string country_code) { //search for the country
    int countryIndex = search(country_code, false, false);
    if(countryIndex == -1 || state[countryIndex] != 1) {
        std::cout << "failure" << std::endl;
        return;
    }

    countries[countryIndex].smallest(); //print the series code with the smallest mean
}

void Data::range(std::string series_code) { 
    double minMean = -1.0; //tracks smallest and largest so far
    double maxMean = -1.0;

    for(int i{0}; i < 512; i++) { //check series for every occupied country slot
        if(state[i] != 1) {
            continue;
        }

        int seriesIndex = countries[i].findSeriesCode(series_code);

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

void Data::build(std::string series_code) {
    clearTree(root); //clear old tree
    root = nullptr;
    currSeriesCode = series_code;

    std::string validCountries[512];
    int numOfValid = 0;
    double minMean = -1.0;
    double maxMean = -1.0;
    
    for(int i{0}; i < 512; i++) { 
        if(state[i] != 1) {
            continue;
        }

        int seriesIndex = countries[i].findSeriesCode(series_code);

        double mean = countries[i].series[seriesIndex]->meanValue(); //calculate matching series mean

        if(mean <= 0) { //ignore invalid
            continue;
        }

        validCountries[numOfValid] = countries[i].getCountryName(); //keep valid country name
        numOfValid++;

        if(minMean == -1.0) { //first valid sets min and max mean so far
            minMean = mean;
            maxMean = mean;
        }

        if(minMean > mean) { //update min
            minMean = mean;
        }
        else if(maxMean < mean) { //update max
            maxMean = mean;
        }
    }

    if(numOfValid > 0) {
        root = recursiveBuild(validCountries, minMean, maxMean, numOfValid, series_code);
    }
    std::cout << "success" << std::endl;
}

TreeNode* Data::recursiveBuild(std::string validCountries[], double minMean, double maxMean, int numOfValid, std::string series_code) {
    TreeNode* node = new TreeNode(minMean, maxMean);

    for(int i{0}; i < numOfValid; i++) {
        if(node->numOfCountries == node->capacity) { //expand dynamic array if full
            node->resize(node->capacity * 2);
        }

        node->countries[node->numOfCountries] = validCountries[i]; //copy next country name
        node->numOfCountries++;
    }

    if(numOfValid == 1) { //if only one country, its a leaf
        return node;
    }

    double firstMean = -1.0;
    bool close = true;

    for(int i{0}; i < numOfValid; i++) {
        double mean = -1.0;

        for(int j{0}; j < 512; j++) { //find countrys mean from the main array
            if(state[j] == 1 && countries[j].getCountryName() == validCountries[i]) {
                int seriesIndex = countries[j].findSeriesCode(series_code);
                mean = countries[j].series[seriesIndex]->meanValue();

                break;
            }
        }

        if(i == 0) {
            firstMean = mean;
        }
        else {
            double diff = mean - firstMean; //compute absolute difference
            if(diff < 0) {
                diff = -diff;
            }
            if(diff > 1e-3) { //if differs too much, node is not counted as a leaf
                close = false;
                break;
            }
        }
    }

    if(close) {
        return node;
    }

    double mid = (minMean + maxMean)/2.0; //split interval
    std::string leftCountries[512];
    std::string rightCountries[512];
    int numOfLeft = 0;
    int numOfRight = 0;

    for(int i{0}; i < numOfValid; i++) { //divide countries based on their mean vs midpoint
        double mean = -1.0;

        for(int j{0}; j < 512; j++) {
            if(state[j] == 1 && countries[j].getCountryName() == validCountries[i]) {
                int seriesIndex = countries[j].findSeriesCode(series_code);
                mean = countries[j].series[seriesIndex]->meanValue();

                break;
            }
        }

        if(mean < mid) { //less than midpoint goes left
            leftCountries[numOfLeft] = validCountries[i];
            numOfLeft++;
        }
        else { //more than midpoint goes right
            rightCountries[numOfRight] = validCountries[i];
            numOfRight++;
        }
    }

    if(numOfLeft == 0 || numOfRight == 0) {
        return node;
    }

    node->left = recursiveBuild(leftCountries, minMean, mid, numOfLeft, series_code);
    node->right = recursiveBuild(rightCountries, mid, maxMean, numOfRight, series_code);
    return node;
}

void Data::find(double mean, std::string operation) { 
    if(root == nullptr) {
        std::cout << "failure" << std::endl;
        return;
    }

    bool first = true;

    recursiveFind(root, mean, operation, first);
    std::cout << std::endl;
}

void Data::recursiveFind(TreeNode* node, double mean, std::string operation, bool& first) {
    if(node == nullptr) {
        return;
    }

    //if whole interval is invalid based on operation, skip
    if(operation == "less" && node->min >= mean) {
        return;
    }

    if(operation == "greater" && node->max <= mean) {
        return;
    }

    if(operation == "equal" && (node->max < mean - 1e-3 || node->min > mean + 1e-3)) {
        return;
    }

    if(node->left == nullptr && node->right == nullptr) { //if this is a leaf, check the country means in it
        for(int i{0}; i < node->numOfCountries; i++) {
            double currMean = -1.0; //mean of leaf country

            for(int j{0}; j < 512; j++) {
                if(state[j] == 1 && countries[j].getCountryName() == node->countries[i]) {
                    int seriesIndex = countries[j].findSeriesCode(currSeriesCode);
                    currMean = countries[j].series[seriesIndex]->meanValue();
                    break;
                }
            }

            //print if satisfies
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

    recursiveFind(node->left, mean, operation, first);
    recursiveFind(node->right, mean, operation, first);
}

void Data::deleteCountry(std::string country_name) {
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

bool Data::recursiveDelete(TreeNode* node, std::string country_name) {
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

void Data::limits(std::string condition) {
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

void Data::trace(std::string country_name) {
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

void Data::recursiveTrace(TreeNode* node, std::string country_name) {
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

bool Data::hasCountry(TreeNode* node, std::string country_name) {
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

void Data::lookup(std::string country_code) {
    search(country_code, false, true);
}

void Data::remove(std::string country_code) {
    int countryIndex = search(country_code, false, false);

    if(countryIndex != -1 && state[countryIndex] == 1) {
        std::string country_name = countries[countryIndex].getCountryName();

        countries[countryIndex].clear();
        state[countryIndex] = -1;
        numOfCountries--;
    
        if(root != nullptr) {
            bool found = recursiveDelete(root, country_name);
    
            if(root != nullptr && root->left == nullptr &&
               root->right == nullptr && root->numOfCountries == 0) {
                delete root;
                root = nullptr;
            }
        }
    
        std::cout << "success" << std::endl;
    }
    else {
        std::cout << "failure" << std::endl;
    }
}

void Data::insert(std::string country_code) {
    int countryIndex = search(country_code, true, false);

    if(countryIndex == -1 || state[countryIndex] == 1) {
        std::cout << "failure" << std::endl;
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

        countries[countryIndex].addSeriesFromRow(line);
    }

    if(found) {
        std::cout << "success" << std::endl;
    }
    else {
        std::cout << "failure" << std::endl;
    }
}

void Data::insertHelper(std::string country_code) {
    int countryIndex = search(country_code, true, false);

    if(countryIndex == -1 || state[countryIndex] == 1) {
        std::cout << "failure" << std::endl;
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

        countries[countryIndex].addSeriesFromRow(line);
    }
}

void Data::clean() {
    std::string temp[512];
    int count = 0;

    for(int i{0}; i < 512; i++) {
        if(state[i] == 1) {
            temp[count] = countries[i].getCountryCode();
            count++;
        }
    }

    for(int i{1}; i < count; i++) {
        std::string key = temp[i];
        int j = i - 1;

        while(j >= 0 && temp[j] > key) {
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

    for(int i{0}; i < count; i++) {
        insertHelper(temp[i]);
    }

    std::cout << "success" << std::endl;
}
