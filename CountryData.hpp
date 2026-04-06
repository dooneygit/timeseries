#pragma once
#include <string>
#include "TimeSeries.hpp"

class Data;

class CountryData {
    friend class Data;

private:
    int numOfElements;
    int capacity;
    TimeSeries** series;
    std::string countryName;
    std::string countryCode;

    void resize(int newCapacity);
    int findSeriesCode(std::string series_code);
public:
    CountryData();
    ~CountryData();
    CountryData(const CountryData&) = delete;
    CountryData& operator=(const CountryData&) = delete;
    void clear();
    void setCountryName(std::string name);
    void setCountryCode(std::string code);
    void addSeriesFromRow(std::string row);
    void load(std::string country_name);
    void list();
    void add(std::string series_code, int y, double d);
    void update(std::string series_code, int y, double d);
    void print(std::string series_code);
    void deleteSeries(std::string series_code);
    void biggest();
    void ts(std::string series_code);
    std::string getCountryName();
    std::string getCountryCode();
    void smallest();
};