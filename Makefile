all: main.cpp TimeSeries.cpp CountryData.cpp
	g++ -std=c++17 main.cpp TimeSeries.cpp CountryData.cpp
	