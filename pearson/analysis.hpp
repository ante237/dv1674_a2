/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "vector.hpp"
#include <vector>

#define MAX_THREADS 8

#if !defined(ANALYSIS_HPP)
#define ANALYSIS_HPP

struct Indexes
{
    int vec1;
    int vec2;
};

struct CalcData
{
    Indexes index[MAX_THREADS];
};

struct ThreadArgs
{
    int tid;
    size_t* chunksPerThread;
    size_t* vecSize;
    std::vector<Vector>* dataset;
    std::vector<double>* res;
    CalcData* data;
};

namespace Analysis {
std::vector<double> correlation_coefficients(std::vector<Vector> datasets);
std::vector<double> correlation_coefficients_par(std::vector<Vector> datasets);
void* threadWorks(void* args);
double pearson(Vector vec1, Vector vec2);
void* pearson_par(std::vector<Vector>* dataset, std::vector<double>* res, CalcData* data, int chunkNr, size_t* vecSize);
};

#endif
