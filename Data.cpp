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

        if(index == -1) {
            index = numOfCountries
            countries[countryIndex].clear()
            countries[countryIndex].setCountryName(name);
            countries[countryIndex].setCountryCode(code);
            numOfCountries++;
        }

        countries[index].addSeriesFromRow(line);
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

void data::country_min(std::string country_code) {
    for(int i{0}; i < numOfCountries; i++) {
        if(countries[i].getCountryCode() == countryCode) {
            countries[i].smallest();
            return;
        }
    }

    std::cout << "failure" << std::endl;
}

void data::range(std::string series_code) {
    double minMean = -1.0;
    double maxMean = -1.0;

    for(int i{0}; i < numOfCountries; i++) {
        int seriesIndex = countries[i].findSeriesCode(seriesCode);
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

    std::string validCountries[512];
    int numOfValidCountries = 0;
    double minMean = -1.0;
    double maxMean = -1.0;
    
    for(int i{0}; i < numOfCountries; i++) {
        int seriesIndex = countries[i].findSeriesCode(seriesCode);
        double mean = countries[i].series[seriesIndex]->meanValue();

        if(mean <= 0) {
            continue;
        }

        validCountries[numOfValidCountries] = countries[i].getCountryName();
        validCount++;

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

    root = recursiveBuild(validCountries, minMean, maxMean, numOfValidCountries, seriesIndex);
    std::cout << "success" << std::endl
}

TreeNode* Data::recursiveBuild(std::string validCountries[], double minMean, double maxMean, int numOfValidCountries, int seriesIndex) {
    TreeNode* node = new TreeNode(minMean, maxMean);

    for(int i{0}; i < numOfValidCountries) {
        node->
    }
}
