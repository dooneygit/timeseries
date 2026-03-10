#include "TimeSeries.hpp"
#include "CountryData.hpp"

class Data {
private:
    TreeNode* root;
    CountryData countries[512];
    int numOfCountries;
    std::string currSeriesCode;

public:
    void load();

};