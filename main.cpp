#include <iostream>
#include <string>
#include "Data.hpp"

int main() {
    Data data;
    std::string cmd;

    while(std::cin >> cmd) {
        if(cmd == "EXIT") {
            break;
        }

        if(cmd == "LOAD_P3") {
            data.load();
        }
        else if(cmd == "LIST_P3") {
            std::string country_name;
            std::getline(std::cin >> std::ws, country_name);
            data.list(country_name);
        }
        else if(cmd == "COUNTRY_MIN_P3") {
            std::string country_code;
            std::cin >> country_code;
            data.country_min(country_code);
        }
        else if(cmd == "RANGE_P3") {
            std::string series_code;
            std::cin >> series_code;
            data.range(series_code);
        }
        else if(cmd == "BUILD_P3") {
            std::string series_code;
            std::cin >> series_code;
            data.build(series_code);
        }
        else if(cmd == "FIND_P3") {
            double mean;
            std::string operation;
            std::cin >> mean >> operation;
            data.find(mean, operation);
        }
        else if(cmd == "DELETE_P3") {
            std::string country_name;
            std::getline(std::cin >> std::ws, country_name);
            data.deleteCountry(country_name);
        }
        else if(cmd == "LIMITS_P3") {
            std::string condition;
            std::cin >> condition;
            data.limits(condition);
        }
        else if(cmd == "TRACE_P3") {
            std::string country_name;
            std::getline(std::cin >> std::ws, country_name);
            data.trace(country_name);
        }
    }

    return 0;
}