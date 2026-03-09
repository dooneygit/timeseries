#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "TimeSeries.hpp"
#include "CountryData.hpp"

void CountryData::resize(int newCapacity) {
    if(newCapacity < 2) { 
        newCapacity = 2; //the smallest that an array can be is 2 elements, regardless of what capacity is passed in
    }

    //allocate new array for elements to be copied over 
    TimeSeries** newSeries = new TimeSeries*[newCapacity];
    
    //move all elements from our old arrays to our new arrays
    for(int i{0}; i < numOfElements; i++) {
        newSeries[i] = series[i];
    }

    //deallocate our old array since they are no longer used and set our pointers to the new array
    delete[] series;
    series = newSeries;
    capacity = newCapacity;
}

int CountryData::findSeriesCode(std::string series_code) {
    for(int i{0}; i < numOfElements; i++) { //iterates through all series for the country
        if(series[i]->getSeriesCode() == series_code) { //if the code of our current series matches, return the index
            return i;
        }
    }

    return -1;
}

CountryData::CountryData(): numOfElements{0}, capacity{2}, series(new TimeSeries*[capacity]) {}

CountryData::~CountryData() {
    for(int i{0}; i < numOfElements; i++) {
        delete series[i]; //delete all series 
    }
    
    delete[] series;
}

void CountryData::load(std::string country_name) {
    for(int i{0}; i < numOfElements; i++) { //delete all series in case load is called again
        delete series[i];
    }
    numOfElements = 0;
    resize(2);

    std::ifstream inputFile("lab2_multidata.csv");
    std::string line;
    bool isCountryFound = false;

    while(std::getline(inputFile, line)) { //read one row at a time
        std::stringstream ss(line);
        std::string name, code;
        std::getline(ss, name, ','); //get country name

        if(!isCountryFound) { //if we haven't gotten to the lines our country encompasses
            if(name != country_name) { //continue until we have our target country
                continue;
            }

            isCountryFound =  true; //we are at the lines our country encompasses
            countryName = name;
            std::getline(ss, code, ','); //get country code
            countryCode = code;
        }
        else {
            if(name != country_name) { //if we are finished with our country, break
                break;
            }
        }

        if (numOfElements == capacity) {
            resize(capacity * 2);
        }
        series[numOfElements] = new TimeSeries();
        series[numOfElements]->loadFromRow(line);
        numOfElements++;
    }

    std::cout << "success" << std::endl;
}

void CountryData::list() {
    std::cout << countryName << " " << countryCode;

    for(int i{0}; i < numOfElements; i++){ //iterate through all series, get and print name
        std::cout << " " << series[i]->getSeriesName();
    }

    std::cout << std::endl;
}

void CountryData::add(std::string series_code, int y, double d) {
    int seriesIndex = findSeriesCode(series_code);

    if(seriesIndex != -1) { //if the series code exists
        series[seriesIndex]->add(y,d);
        return;
    }

    std::cout << "failure" << std::endl;
}

void CountryData::update(std::string series_code, int y, double d) {
    int seriesIndex = findSeriesCode(series_code);

    if(seriesIndex != -1) { //if the series code exists
        series[seriesIndex]->update(y,d);
        return;
    }

    std::cout << "failure" << std::endl;
}

void CountryData::print(std::string series_code) {
    int seriesIndex = findSeriesCode(series_code);

    if(seriesIndex != -1) { //if the series code exists
        series[seriesIndex]->print();
        return;
    }

    std::cout << "failure" << std::endl;
}

void CountryData::deleteSeries(std::string series_code) {
    int seriesIndex = findSeriesCode(series_code);

    if(seriesIndex != -1) {
        delete series[seriesIndex]; //delete series

        for(int i = seriesIndex + 1; i < numOfElements; i++) { //shift all series left to fill empty space
            series[i - 1] = series[i];
        }
        numOfElements--;

        if(capacity > 2 && numOfElements == capacity/4) { 
            resize(capacity/2);
        }

        std::cout << "success" << std::endl;
        return;
    }

    std::cout << "failure" << std::endl;
}

void CountryData::biggest() {
    int biggestIndex = -1;
    double biggestMean = 0.0;

    for(int i{0}; i < numOfElements; i++) {
        if(series[i]->getSize() == 0) { //if the series has no valid data, skip
            continue;
        }
        double mean = series[i]->meanValue(); //calculate mean
        if(biggestIndex == -1 || mean > biggestMean) { //if we have no biggest mean or if our mean is the biggest so far
            biggestIndex = i; //set to current index
            biggestMean = mean;
        }
    }

    if(biggestIndex == -1) {
        std::cout << "failure" << std::endl; //if we never set a biggest mean, no series has valid data
        return;
    }

    std::cout << series[biggestIndex]->getSeriesCode() << std::endl;
}

void CountryData::ts(std::string series_code) {
    int seriesIndex = findSeriesCode(series_code);

    if(seriesIndex != -1) {
        int seriesSize = series[seriesIndex]->getSize();
        int seriesCapacity = series[seriesIndex]->getCapacity();

        std::cout << "size is " << seriesSize << " capacity is " << seriesCapacity << std::endl;
        return;
    }

    std::cout << "failure" << std::endl;
}

void CountryData::getCountryName() {
    return countryName;
}

void CountryData::getCountryCode() {
    return countryCode;
}

void CountryData::smallest() {
    int smallestIndex = -1;
    double smallestMean = 0.0;

    for(int i{0}; i < numOfElements; i++) {
        if(series[i]->getSize() == 0) { 
            continue;
        }

        double mean = series[i]->meanValue();
        if(smallestIndex == -1 || mean < smallestMean) {
            smallestIndex = i;
            smallestMean = mean;
        }
    }

    if(smallestIndex == -1) {
        std::cout << "failure" << std::endl;
        return;
    }

    std::cout << series[smallestIndex]->getSeriesCode() << std::endl;
}





