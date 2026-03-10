#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Data.hpp"
#include "TimeSeries.hpp"
#include "CountryData.hpp"

void Data::load() {
    for(int i{0}; i < numOfCountries; i++) {
        countries[i].clear();
    }
    numOfCountries = 0;

    std::ifstream inputFile("lab2_multidata.csv");
    std::string line;

    std::getline(inputFile, line);

    while(std::getline(inputFile, line)) {
        std::stringstream ss(line);;
        std::string name, code;

        std::getline(ss, name, ',');
        std::getline(ss, code, ',');

        int countryIndex = -1

        for(int i{0}; i < numOfCountries; i++) {
            if(countries[i].getCountryName() == name && countries[i].getCountryCode() == code) {
                countryIndex = i;
                break;
            }
        }

        if(countryIndex == -1) {
            index = numOfCountries
            countries[countryIndex].clear()
            countries[countryIndex].setCountryName(name);
            countries[countryIndex].setCountryCode(code);
            numOfCountries++;
        }

        countries[countryIndex].addSeriesFromRow(line);
    }

    std::cout << "success" << std::endl;
}

void Data::list(const std::string country_name) {
    for(int i{0}; i < numOfCountries; i++) {
        if(countries[i].getCountryName() == country_name) {
            countries[i].list();
            return;
        }
    }
}

void Data::country_min(std::string country_code) {
    for(int i{0}; i < numOfCountries; i++) {
        if(countries[i].getCountryCode() == country_code) {
            countries[i].smallest();
            return;
        }
    }

    std::cout << "failure" << std::endl;
}

void Data::range(std::string series_code) {
    double minMean = -1.0;
    double maxMean = -1.0;

    for(int i{0}; i < numOfCountries; i++) {
        int seriesIndex = countries[i].findSeriesCode(series_code);
        double mean = countries[i].series[seriesIndex]->meanValue();
        
        if(mean <= 0) {
            continue;
        }

        if(minMean =< 0) {
            minMean = mean;
            maxMean = mean;
        }

        if(minMean > mean) {
            minMean = mean;
        }
        else if (mean > maxMean) {
            maxMean = mean;
        }
    }

    std::cout << minMean << " " << maxMean << std::endl;
}

void Data::build(std::string series_code) {
    clearTree(root);
    root = nullptr;
    currSeriesCode = series_code

    std::string validCountries[512];
    int numOfValid = 0;
    double minMean = -1.0;
    double maxMean = -1.0;
    
    for(int i{0}; i < numOfCountries; i++) {
        int seriesIndex = countries[i].findSeriesCode(series_code);
        double mean = countries[i].series[seriesIndex]->meanValue();

        if(mean <= 0) {
            continue;
        }

        validCountries[numOfValid] = countries[i].getCountryName();
        numOfValid++;

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

    root = recursiveBuild(validCountries, minMean, maxMean, numOfValid, series_code);
    std::cout << "success" << std::endl
}

TreeNode* Data::recursiveBuild(std::string validCountries[], double minMean, double maxMean, int numOfValid, std::string series_code) {
    TreeNode* node = new TreeNode(minMean, maxMean);

    for(int i{0}; i < numOfValid;i++) {
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
        double mean = -1.0;

        for(int j{0}; j < numOfCountries; j++) {
            if(countries[j].getCountryName() == validCountries[i]) {
                int seriesIndex = countries[j].findSeriesCode(series_code);
                mean = countries[j].series[seriesIndex]->meanValue();

                break;
            }
        }

        if(i == 0) {
            firstMean = mean;
        }
        else{
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
    std::string leftCountries[512];
    std::string rightCountries[512];
    int numOfLeft = 0;
    int numOfRight = 0;

    for(int i{0}; i < numOfValid; i++) {
        double mean = -1.0;

        for(int j{0}; j < numOfCountries; j++) {
            if(countries[j].getCountryName() == validCountries[i]) {
                int seriesIndex = countries[j].findSeriesCode(series_code);
                mean = countries[j].series[seriesIndex]->meanValue();
                break;
            }
        }

        if(mean < mid) {
            leftCountries[numOfLeft] = validCountries[i];
            numOfLeft++;
        }
        else {
            rightCountries[numOfRight] = validCountries[i];
            numOfRight++;
        }
    }

    if(numOfLeft == 0 || numOfRight == 0) {
        return node;
    }

    node->left = recursiveBuild(series_code, minMean, leftCountries, numOfLeft, mid);
    node->left = recursiveBuild(series_code, maxMean, rightCountries, numOfRight, mid);
    return node;
}

void Data::find(double mean, std::string operation) {
    if(root == nullptr) {
        std::cout << "failure" << std::endl;
        return;
    }

    bool first = true;

    recursiveFind(root, first, mean, operation);
    std::cout << std::endl;
}

void Data::recursiveFind(TreeNode* node, double mean, std::string operation, bool& first) {
    if(node == nullptr) {
        return;
    }

    if(operation == "less" && node->min >= mean) {
        return;
    }

    if(operation == "greater" && node->max <= mean) {
        return;
    }

    if(operation == "equal" && (node->max < mean - 1e-3 || node-> min > mean + 1e-3)) {
        return;
    }

    if(node->left == nullptr && node->right == nullptr) {
        for(int i{0}; i < node->numOfCountries; i++) {
            double currMean = -1.0;

            for(int j{0}; j < numOfCountries; j++) {
                int seriesIndex = countries[j].findSeriesCode(currSeriesCode);
                currMean = countries[j].series[seriesIndex]->meanValue();
                break;
            }
        }

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
                    std::cout <<< " ";
                }
                std::cout << node->countries[i];
                first = false;
            }
        }

        return;
    }

    recursiveFind(node->left, mean, operation, first);
    recursiveFind(node->right, mean, operation, first);
}


