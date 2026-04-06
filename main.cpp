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

        else if(cmd == "LOAD_P3" || cmd == "LOAD_P4") {
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
        else if(cmd == "LOOKUP_P4") {
            std::string country_code;
            std::cin >> country_code;
            data.lookup(country_code);
        }
        else if(cmd == "INSERT_P4") {
            std::string country_code;
            std::cin >> country_code;
            data.insert(country_code);
        }
        else if(cmd == "REMOVE_P4") {
            std::string country_code;
            std::cin >> country_code;
            data.remove(country_code);
        }
        else if(cmd == "CLEAN_P4") {
            data.clean();
        }
        else if(cmd == "INITIALIZE_P5") {
            data.initialize();
        }
        else if(cmd == "UPDATE_EDGES_P5") {
            std::string series_code, relation;
            double threshold;
            std::cin >> series_code >> threshold >> relation;
            data.update_edges(series_code, threshold, relation);
        }
        else if(cmd == "ADJACENT_P5") {
            std::string country_code;
            std::cin >> country_code;
            data.adjacent(country_code);
        }
        else if(cmd == "PATH_P5") {
            std::string code1, code2;
            std::cin >> code1 >> code2;
            data.path(code1, code2);
        }
        else if(cmd == "RELATIONSHIPS_P5") {
            std::string code1, code2;
            std::cin >> code1 >> code2;
            data.relationships(code1, code2);
        }
    }

    return 0;
}