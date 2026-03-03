#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "TimeSeries.hpp"

void TimeSeries::resize(int newCapacity) {
    if(newCapacity < 2) { 
        newCapacity = 2; //the smallest that an array can be is 2 elements, regardless of what capacity is passed in
    }

    //allocate new arrays for elements to be copied over 
    int* newYears = new int[newCapacity]; 
    double* newData = new double[newCapacity];
    
    //move all elements from our old arrays to our new arrays
    for(int i{0}; i < numOfElements; i++) {
        newYears[i] = years[i];
        newData[i] = data[i];
    }

    //deallocate our old arrays since they are no longer used and set our pointers to the new arrays
    delete[] years;
    years = newYears;
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

int TimeSeries::findYearIndex(int y) { //binary search
    int left = 0; //left boundary
    int right = numOfElements - 1; //right boundary
    int average = 0;

    while(left <= right) {
        average = left + (right - left)/2; //calculate the middle of the boundary

        if(years[average] == y) { //if we find our target year, early return the index
            return average;
        }

        if(years[average] < y) { //if our result is lower than our target, search the upper half
            left = average + 1;
        }
        else {
            right = average - 1; //if our result is higher than our target, search the lower half
        }
    }
    
    return -1; //return -1 if not found
}

TimeSeries::TimeSeries(): numOfElements{0}, capacity{2}, years(new int[capacity]), data(new double[capacity]) {} //constructor allocates our arrays

TimeSeries::~TimeSeries() { //destructor deallocates our arrays
    delete[] years; 
    delete[] data; 
}

void TimeSeries::load(std::string filename) {
    //resets our numOfElements and resizes to the initial capacity of 2 in case load is called again
    numOfElements = 0;
    resize(2);

    std::ifstream inputFile(filename);
    std::string token;
    int index = 0;
    int year = 1960;
    
    while (std::getline(inputFile, token, ',')) {
        
        if (index >= 4) { //our data always starts at an index of 4 in the csv files, so we only need to store when index >= 4
            std::stringstream entry(token);
            double value;
            entry >> value;

            if (value != -1) { //if our data is not invalid, we check if we need to resize and then input our data into the array
                if (numOfElements == capacity) {
                    resize(capacity * 2);
                }

                years[numOfElements] = year;
                data[numOfElements]  = value;
                numOfElements++; //keeps track of how many elements we have, only incrementing after adding an element
            }
            year++; //we start incrementing year from 1960 only after we've ignored the first 4 elements (not data)
        }
        index++; //index always increments at every iteration
    }

    std::cout << "success" << std::endl;
}

void TimeSeries::loadFromRow(std::string row) {
    //resets our numOfElements and resizes to the initial capacity of 2 in case load is called again
    numOfElements = 0;
    resize(2);

    std::stringstream ss(row);
    std::string token;
    int index = 0;
    int year = 1960;
    
    while (std::getline(ss, token, ',')) {
        if(index == 2) {
            seriesName = token;
        }
        else if (index == 3) {
            seriesCode = token;
        }
        else if (index >= 4) { //our data always starts at an index of 4 in the csv files, so we only need to store when index >= 4
            std::stringstream entry(token);
            double value;
            entry >> value;

            if(value >= 0) { //if our data is not invalid, we check if we need to resize and then input our data into the array
                if (numOfElements == capacity) {
                    resize(capacity * 2);
                }

                years[numOfElements] = year;
                data[numOfElements]  = value;
                numOfElements++; //keeps track of how many elements we have, only incrementing after adding an element
            }
            year++; //we start incrementing year from 1960 only after we've ignored the first 4 elements (not data)
        }
        index++; //index always increments at every iteration
    }
}

void TimeSeries::print() {
    if(numOfElements > 0) { //if there is any valid data, we print them out in pairs
        for(int i{0}; i < numOfElements; i++) {
            std::cout << "(" << years[i] << "," << data[i] << ") ";
        }
        std::cout << std::endl;
    }
    else { //if numOfElements < 0, that means we have no valid data and print fails
        std::cout << "failure" << std::endl;
    }
}

void TimeSeries::add(int y, double d) {
    if(findYearIndex(y) == -1){ //if this year doesn't have valid data 
        
        //if we've reached the end of our capacity, double it
        if(numOfElements == capacity) { 
            resize(capacity*2);
        }

        int insertIndex = 0;
        //Finds our index to insert at.
        //Our index has to be within the number of elements we have and increment index until the year is greater than our target year (we have the sorted position)
        while(insertIndex < numOfElements && years[insertIndex] < y) {
            insertIndex++;
        }

        //shift all elements after our insertIndex to the right to make space for it
        for(int i{numOfElements}; i > insertIndex; i--) { 
            years[i] = years[i - 1];
            data[i] = data[i - 1];
        }

        //set the appropriate year and data at our insertIndex
        years[insertIndex] = y;
        data[insertIndex] = d;

        numOfElements++; //add to our number of elements

        std::cout << "success" << std::endl;
    }
    else { //if the year already has data, add fails
        std::cout << "failure" << std::endl;
    }
}

bool TimeSeries::is_monotonic() {
    if(numOfElements == 0) { //if there is no valid data, is_monotonic fails
        std::cout << "failure" << std::endl;
        return false;
    }
    bool isIncreasing = false, isDecreasing = false;

    for(int i{0}; i < numOfElements - 1; i++) {
        if(data[i] < data[i + 1]) { //if the next element is ever greater, that means we are increasing
            isIncreasing = true;
        }
        else if(data[i] > data[i+1]) { //if the next element is ever lesser, that means we are decreasing
            isDecreasing = true;
        }

        //if we have both increased and decreased throughout the array, the series is not monotonic. early return false 
        if(isIncreasing && isDecreasing == true) { 
            std::cout << "series is not monotonic" << std::endl;
            return false;
        }
    }

    std::cout << "series is monotonic" << std::endl;
    return true; //there was never both an increase and decrease in our series, the series is monotonic
}

double TimeSeries::mean() {
    if(numOfElements == 0) { //if there is no valid data, we can not calculate a mean and mean fails
        std::cout << "failure" << std::endl;
        return 0;
    }

    double sum = 0;

    //for every element in our data array, add it to the total sum of all elements
    for(int i{0}; i < numOfElements; i++) {
        sum += data[i];
    }

    std::cout << "mean is " << sum/numOfElements << std::endl;
    return sum/numOfElements; //the mean is our the sum of all our elements divided by the number of elements
}

bool TimeSeries::best_fit(double &m, double &b) {
    if(numOfElements < 2) { //if we do not have enough valid data points (2), then we return m = 0 and b = 0 and best_fit fails
        m = 0.0;
        b = 0.0;
        std::cout << "failure" << std::endl;
        return false; 
    }
    
    double xySum = 0.0, xSum = 0.0, ySum = 0.0, x2Sum = 0.0, N = numOfElements; 

    for(int i{0}; i < numOfElements; i++) {
        double x = years[i]; //x is set to the year at that index every iteration
        double y = data[i]; //y is set to the data at that index every iteration

        xySum += (x * y); //add to the total summation of all our xy every iteration
        xSum += x; //add to the total summation of all our x every iteration
        ySum += y; //add to the total summation of all our y every iteration
        x2Sum += (x * x); //add to the total summation of all our x^2 every iteration
    }

    m = (N * xySum - xSum * ySum)/(N * x2Sum - (xSum * xSum)); //calculates slope
    b = (ySum - m * xSum)/N; //calculates intercept

    std::cout << "slope is " << m << " intercept is " << b << std::endl;
    return true;
}

void TimeSeries::update(int y, double d) {
    int index = findYearIndex(y);

    if(index != -1) { //if that year has valid data
        //if input data is negative, we shift all elements after that index to the left to "override" the existing data
        if(d < 0) {
            for(int i{index + 1}; i < numOfElements; i++) {
                years[i - 1] = years[i];
                data[i - 1] = data[i]; 
            }

            numOfElements--; //subtract from the number of elements
            //if our capacity is more than the minimum but our elements are 1/4 of it, shrink it to half
            if(capacity > 2 && numOfElements == capacity/4) { 
                resize(capacity/2);
            }
            std::cout << "success" << std::endl;
        }
        else { //if data is positive, set the data for that year to input d
            data[index] = d;
            std::cout << "success" << std::endl;
        }
    }
    else{ //of that year has no valid data, we can not update it. update fails
        std::cout << "failure" << std::endl;
    }
}

double TimeSeries::meanValue() {
    if(numOfElements == 0) { //if there is no valid data, we can not calculate a mean and mean fails
        return 0;
    }

    double sum = 0;

    //for every element in our data array, add it to the total sum of all elements
    for(int i{0}; i < numOfElements; i++) {
        sum += data[i];
    }

    return sum/numOfElements; //the mean is our the sum of all our elements divided by the number of elements
}

std::string TimeSeries::getSeriesCode() {
    return seriesCode;
}

std::string TimeSeries::getSeriesName() {
    return seriesName;
}

int TimeSeries::getSize() {
    return numOfElements;
}

int TimeSeries::getCapacity() {
    return capacity;
}