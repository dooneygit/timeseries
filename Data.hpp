#include "TimeSeries.hpp"
#include "CountryData.hpp"

class Data {
private:
    TreeNode* root;
    CountryData countries[512];
    int numOfCountries;

public:
    void load();

};