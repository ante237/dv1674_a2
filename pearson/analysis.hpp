/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "vector.hpp"
#include <vector>

#define MAX_THREADS 8

#if !defined(ANALYSIS_HPP)
#define ANALYSIS_HPP

struct CalcData
{
    int* vec1;
    int* vec2;
};

struct ThreadArgs
{
    int tid;
    size_t* elementsPerThread;
    size_t* vecSize;
    std::vector<Vector>* dataset;
    std::vector<double>* res;
    CalcData* data;
};

namespace Analysis {
std::vector<double> correlation_coefficients(std::vector<Vector> datasets);
std::vector<double> correlation_coefficients_par(std::vector<Vector>* datasets, std::vector<double>* res);
void* threadWorks(void* args);
double pearson(Vector vec1, Vector vec2);
void* pearson_par(std::vector<Vector>* dataset, std::vector<double>* res, CalcData* data, int elementNr, size_t* size);
};

#endif
