#pragma once
#include <string>

class TimeSeries {
private:
    int numOfElements;
    int capacity;
    int* years;
    double* data;
    std::string seriesName;
    std::string seriesCode;

    void resize(int newCapacity);
    int findYearIndex(int y);

public:
    TimeSeries();
    ~TimeSeries();
    TimeSeries(const TimeSeries&) = delete;
    TimeSeries& operator=(const TimeSeries&) = delete;
    void load(std::string filename);
    void loadFromRow(std::string row);
    void print();
    void add(int y, double d);
    void update(int y, double d);
    bool is_monotonic();
    double mean();
    bool best_fit(double& m, double& b);
    int getSize();
    int getCapacity();
    std::string getSeriesCode();
    std::string getSeriesName();
    double meanValue();
};
