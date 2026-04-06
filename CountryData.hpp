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
    int findSeriesCode(const std::string& series_code);
public:
    CountryData();
    ~CountryData();
    CountryData(const CountryData&) = delete;
    CountryData& operator=(const CountryData&) = delete;
    void clear();
    void setCountryName(const std::string& name);
    void setCountryCode(const std::string& code);
    void addSeriesFromRow(const std::string& row);
    void load(const std::string& country_name);
    void list();
    void add(const std::string& series_code, int y, double d);
    void update(const std::string& series_code, int y, double d);
    void print(const std::string& series_code);
    void deleteSeries(const std::string& series_code);
    void biggest();
    void ts(const std::string& series_code);
    std::string getCountryName();
    std::string getCountryCode();
    void smallest();
};