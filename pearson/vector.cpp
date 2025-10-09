/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "vector.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <immintrin.h>

Vector::Vector()
    : size{0}, data{nullptr}
{
}

Vector::~Vector()
{
    if (data)
    {
        delete[] data;
    }

    size = 0;
}

Vector::Vector(unsigned size)
    : size{size}, data{new double[size]}
{
}

Vector::Vector(unsigned size, double *data)
    : size{size}, data{data}
{
}

Vector::Vector(const Vector &other)
    : Vector{other.size}
{
    for (auto i{0}; i < size; i++)
    {
        data[i] = other.data[i];
    }
}

unsigned Vector::get_size() const
{
    return size;
}

double *Vector::get_data()
{
    return data;
}

double Vector::operator[](unsigned i) const
{
    return data[i];
}

double &Vector::operator[](unsigned i)
{
    return data[i];
}

double Vector::mean() const
{
    double sum{0};
    __m256d sumVec = _mm256_setzero_pd();
    for (auto i{0}; i + 3 < size; i += 4)
    {
        __m256d chunk = _mm256_loadu_pd(&data[i]);
        sumVec = _mm256_add_pd(sumVec, chunk);
    }
    alignas(32) double tmp[4];
    _mm256_store_pd(tmp, sumVec);
    sum = tmp[0] + tmp[1] + tmp[2] + tmp[3];

    return sum / static_cast<double>(size);
}

double Vector::magnitude() const
{
    auto dot_prod{dot(*this)};
    return std::sqrt(dot_prod);
}

Vector Vector::operator/(double div)
{
    auto result{*this};

    for (auto i{0}; i < size; i++)
    {
        result[i] /= div;
    }

    return result;
}

Vector Vector::operator-(double sub)
{
    auto result{*this};
    __m256d scalar = _mm256_set1_pd(sub);

    for (auto i{0}; i + 3 < size; i += 4)
    {
        __m256d vecSeg = _mm256_loadu_pd(&(result.data)[i]);
        vecSeg = _mm256_sub_pd(vecSeg, scalar);
        _mm256_storeu_pd(&(result.data)[i], vecSeg);
    }

    return result;
}

double Vector::dot(Vector rhs) const
{
    double result{0};

    for (auto i{0}; i < size; i++)
    {
        result += data[i] * rhs[i];
    }

    return result;
}