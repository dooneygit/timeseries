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

void data::country_min() {
    for(int i{0}; i < numOfCountries; i++) {
        if(countries[i].getCountryCode() == countryCode) {
            countries[i].smallest();
            return;
        }
    }

    std::cout << "faiure" << std::endl;
}

