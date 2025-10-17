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

namespace Dataset
{
    std::vector<Vector> read(std::string filename)
    {
        unsigned dimension{};
        std::vector<Vector> result{};
        std::ifstream f{};

        f.open(filename);

        if (!f)
        {
            std::cerr << "Failed to read dataset(s) from file " << filename << std::endl;
            return result;
        }

        f >> dimension;
        std::string line{};
        std::getline(f, line);
        //Get first line and reserve space required
        result.reserve(dimension);

        while (std::getline(f, line))
        {
            const char* ptr = line.c_str();
            char* end;
            Vector new_vec{dimension};

            for(int i = 0; i < dimension; i++)
            {
                new_vec[i] = std::strtod(ptr, &end);
                ptr = end;
            }
            result.push_back(std::move(new_vec));
        }
        return result;
    }

    
    void write(std::vector<double> data, std::string filename)
    {
        //Set buffer size to fit inside of L2 cache
        constexpr size_t BUF_SIZE = 128 * 1024;
        char writeBuf[BUF_SIZE];
        int offset = 0;
        
        FILE* f;
        f = fopen(filename.c_str(), "w");
        for (auto i{0}; i < data.size(); i++)
        {
            int written = snprintf(writeBuf + offset, sizeof(writeBuf) - offset, "%.*f\n", DBL_DIG + 1, data[i]);
            offset += written;

            //Write buffer if nearly full
            if(offset > BUF_SIZE - 32)
            {
                fwrite(writeBuf, sizeof(char), offset, f);
                offset = 0;
            }
        }
        if(offset > 0) fwrite(writeBuf, sizeof(char), offset, f);

        fclose(f);
    }

};
