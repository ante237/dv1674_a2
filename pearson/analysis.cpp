/*
Author: David Holmqvist <daae19@student.bth.se>
*/

/*
 * Improvements to file
 * - Remove auto
 * - Threads
 */

#include "analysis.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <list>
#include <vector>
#include <pthread.h>

namespace Analysis {

std::vector<double> correlation_coefficients(std::vector<Vector> datasets)
{
    std::vector<double> result {};

    for (auto sample1 { 0 }; sample1 < datasets.size() - 1; sample1++) {
        for (auto sample2 { sample1 + 1 }; sample2 < datasets.size(); sample2++) {
            auto corr { pearson(datasets[sample1], datasets[sample2]) };
            result.push_back(corr);
        }
    }

    return result;
}

std::vector<double> correlation_coefficients_par(std::vector<Vector>* datasets, std::vector<double>* res)
{
    pthread_t threads[MAX_THREADS];
    //Calculate number of resulting elements
    size_t size = datasets->size();
    size_t elementCount = (size * (size - 1)) / 2;
    size_t elementsPerThread = elementCount / 8;
    
    //Preallocate memory for vector, to avoid dynamic reallocations
    std::vector<double> result {};
    result.resize(elementCount);

    CalcData* all = new CalcData;
    all->vec1 = new int[elementCount];
    all->vec2 = new int[elementCount];

    int offset = 0;
    //Change loop so result allways gets split into 64-byte sections to avoid false sharing
    for (int sample1 = 0; sample1 < size - 1; sample1++) {
        std::fill_n(all->vec1 + offset, size-sample1-1, sample1);
        std::iota(all->vec2 + offset, all->vec2 + offset + (size - sample1 - 1), sample1 + 1);

        offset += size-sample1-1;
    }

    //Initialize threads
    ThreadArgs args[MAX_THREADS];
    for(int i = 0; i < MAX_THREADS; i++)
    {
        args[i] = {i, &elementsPerThread, &size, datasets, &result, all};
        pthread_create(&threads[i], NULL, threadWorks, &args[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], nullptr);
    }

    return result;
}

void* threadWorks(void* voidargs)
{
    ThreadArgs* args = (ThreadArgs*)voidargs;
    for(int i = 0; i < *(args->elementsPerThread); i++)
    {
        //Allocate a chunk to each thread, to avoid memory overlap
        pearson_par(args->dataset, args->res, args->data, i + (args->tid * (*(args->elementsPerThread))), args->vecSize);
    }
    return nullptr;
}

void* pearson_par(std::vector<Vector>* dataset, std::vector<double>* res, CalcData* data, int elementNr, size_t* size)
{
    int vec1Index = data->vec1[elementNr];
    int vec2Index = data->vec2[elementNr];

    int resPos = ((vec1Index * (2*(*size) - vec1Index - 1)) / 2) + (vec2Index - vec1Index - 1);
    
    double x_mean = (*dataset)[vec1Index].mean();
    double y_mean = (*dataset)[vec2Index].mean();
    
    Vector x_mm =(*dataset)[vec1Index] - x_mean;
    Vector y_mm =(*dataset)[vec2Index] - y_mean;

    double x_mag =x_mm.magnitude();
    double y_mag =y_mm.magnitude();

    Vector x_mm_over_x_mag =x_mm / x_mag;
    Vector y_mm_over_y_mag =y_mm / y_mag;

    double r =x_mm_over_x_mag.dot(y_mm_over_y_mag);

    (*res)[resPos] = std::max(std::min(r, 1.0), -1.0);
    return nullptr;
}

double pearson(Vector vec1, Vector vec2)
{
    auto x_mean { vec1.mean() };
    auto y_mean { vec2.mean() };

    auto x_mm { vec1 - x_mean };
    auto y_mm { vec2 - y_mean };

    auto x_mag { x_mm.magnitude() };
    auto y_mag { y_mm.magnitude() };

    auto x_mm_over_x_mag { x_mm / x_mag };
    auto y_mm_over_y_mag { y_mm / y_mag };

    auto r { x_mm_over_x_mag.dot(y_mm_over_y_mag) };

    return std::max(std::min(r, 1.0), -1.0);
}
};
