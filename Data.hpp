#include "TimeSeries.hpp"
#include "CountryData.hpp"

class Data {
private:
    CountryData countries[512];
    int numOfCountries;

public:
    void load();

};