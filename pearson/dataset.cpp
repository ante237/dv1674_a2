/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "dataset.hpp"
#include "vector.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <stdio.h>
#include <float.h>

#define IN_DECIMALS 3

namespace Dataset
{
    std::vector<Vector> read(std::string filename)
    {
        FILE* f;
        unsigned dimension{};
        std::vector<Vector> result{};
        char tmp[16];
        f = fopen(filename.c_str(), "r");
        
        if (f == NULL)
        {
            std::cerr << "Failed to read dataset(s) from file " << filename << std::endl;
            return result;
        }

        fgets(tmp, sizeof(tmp), f);
        dimension = strtol(tmp, nullptr, 10);
        //Reserve space required for result vector
        result.reserve(dimension);
        
        //Calculate size of buffer to fit a full line into a single buffer (add 1 for nullterminator)
        const size_t BUF_SIZE = (3 + IN_DECIMALS) * dimension + 1;
        char readBuf[BUF_SIZE];
        while(fgets(readBuf, sizeof(readBuf), f))
        {
            char* ptr = readBuf;
            char* pEnd;
            Vector new_vec{dimension};

            for(int i = 0; i < dimension; i++)
            {
                new_vec[i] = std::strtod(ptr, &pEnd);
                ptr = pEnd;
            }
            result.push_back(std::move(new_vec));
        }
        fclose(f);
        return result;
    }


    void write(std::vector<double> data, std::string filename)
    {
        //Set buffer size to fit inside of L2 cache
        constexpr size_t BUF_SIZE = 128 * 1024;
        char writeBuf[BUF_SIZE];
        int offset = 0;
        
        FILE* pFile;
        pFile = fopen(filename.c_str(), "w");
        for (auto i{0}; i < data.size(); i++)
        {
            int written = snprintf(writeBuf + offset, sizeof(writeBuf) - offset, "%.*f\n", DBL_DIG + 1, data[i]);
            offset += written;

            //Write buffer if nearly full
            if(offset > BUF_SIZE - 32)
            {
                fwrite(writeBuf, sizeof(char), offset, pFile);
                offset = 0;
            }
        }
        if(offset > 0) fwrite(writeBuf, sizeof(char), offset, pFile);

        fclose(pFile);
    }

};
