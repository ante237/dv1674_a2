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

std::vector<double> correlation_coefficients_par(std::vector<Vector> datasets)
{
    pthread_t threads[MAX_THREADS];
    //Calculate number of resulting elements
    size_t size = datasets.size();
    size_t elementCount = (size * (size - 1)) / 2;
    
    //Preallocate memory for vector, to avoid dynamic reallocations
    std::vector<double> result {};
    result.resize(elementCount);

    size_t chunkCount = (int)floor(elementCount / MAX_THREADS);
    size_t chunksPerThread = chunkCount / MAX_THREADS;
    CalcData* all = new CalcData[chunkCount + 1];
    int counter = 0;

    //Change loop so result allways gets split into 64-byte sections to avoid false sharing
    for (int sample1 = 0; sample1 < datasets.size() - 1; sample1++) {
        for (int sample2 = sample1 + 1; sample2 < datasets.size(); sample2++) {
            all[(int)(counter / MAX_THREADS)].index[counter % MAX_THREADS].vec1 = sample1;
            all[(int)(counter / MAX_THREADS)].index[counter % MAX_THREADS].vec2 = sample2;
            
            counter++;
        }
    }
    //Initialize threads
    ThreadArgs args[MAX_THREADS];
    for(int i = 0; i < MAX_THREADS; i++)
    {
        args[i] = {i, &chunksPerThread, &size, &datasets, &result, all};
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
    for(int i = 0; i < *(args->chunksPerThread); i++)
    {
        //Allocate a chunk to each thread, to avoid memory overlap
        pearson_par(args->dataset, args->res, args->data, i + (args->tid * (*(args->chunksPerThread))), args->vecSize);
    }
    return nullptr;
}

void* pearson_par(std::vector<Vector>* dataset, std::vector<double>* res, CalcData* data, int chunkNr, size_t vecSize)
{
    for(int i = 0; i < MAX_THREADS; i++)
    {
        int resPos = data[chunkNr].index[i].vec1 * vecSize + (data[chunkNr].index[i].vec2 - 1);

        double x_mean = (*dataset)[data[chunkNr].index[i].vec1].mean();
        double y_mean = (*dataset)[data[chunkNr].index[i].vec2].mean();
        
        Vector x_mm =(*dataset)[data[chunkNr].index[i].vec1] - x_mean;
        Vector y_mm =(*dataset)[data[chunkNr].index[i].vec2] - y_mean;

        double x_mag =x_mm.magnitude();
        double y_mag =y_mm.magnitude();

        Vector x_mm_over_x_mag =x_mm / x_mag;
        Vector y_mm_over_y_mag =y_mm / y_mag;

        double r =x_mm_over_x_mag.dot(y_mm_over_y_mag);

        if(data[chunkNr].index[i].vec1 == -1) continue;
        (*res)[resPos] = std::max(std::min(r, 1.0), -1.0);
    }
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
