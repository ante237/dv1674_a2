/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "analysis.hpp"
#include "dataset.hpp"
#include <iostream>
#include <cstdlib>

#include <chrono>

int main(int argc, char const* argv[])
{
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [dataset] [outfile] [num_threads]" << std::endl;
        std::exit(1);
    }

    auto datasets { Dataset::read(argv[1]) };
    auto corrs { Analysis::correlation_coefficients_par(datasets, atoi(argv[3])) };
    auto printStart = high_resolution_clock::now();
    Dataset::write(corrs, argv[2]);

    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    auto printTime = duration_cast<microseconds>(end - printStart);
    std::cout << "Time elapsed: " << duration.count() << " µs\n";
    std::cout << "Print time: " << printTime.count() << " µs\n";

    return 0;
}
