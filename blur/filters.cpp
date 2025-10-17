/*
Author: David Holmqvist <daae19@student.bth.se>
*/


/*
 * Improvements to file
 * - Threads
 */

#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <cmath>
#include <pthread.h>

namespace Filter
{
    namespace Gauss
    {
        void get_weights(int n, double *weights_out)
        {
            for (auto i{0}; i <= n; i++)
            {
                double x{static_cast<double>(i) * max_x / n};
                weights_out[i] = exp(-x * x * pi);
            }
        }
    }

    struct BlurArgs {
        Matrix *src;
        Matrix *dst;
        Matrix *scratch;
        int radius;
        int start_x;
        int end_x;
        int start_y;
        int end_y;
    };

    void* blur_horizontal(void* arg)
    {
        auto* args = static_cast<BlurArgs*>(arg);
        auto& src = *args->src;
        auto& scratch = *args->scratch;
        int radius = args->radius;

        double w[Gauss::max_radius]{};
        Gauss::get_weights(radius, w);

        for (int x = args->start_x; x < args->end_x; x++) {
            for (int y = args->start_y; y < args->end_y; y++) {
                auto r{w[0] * src.r(x, y)}, g{w[0] * src.g(x, y)}, b{w[0] * src.b(x, y)}, n{w[0]};

                for (int wi = 1; wi <= radius; wi++) {
                    auto wc = w[wi];
                    int x2 = x - wi;
                    if (x2 >= 0) {
                        r += wc * src.r(x2, y);
                        g += wc * src.g(x2, y);
                        b += wc * src.b(x2, y);
                        n += wc;
                    }
                    x2 = x + wi;
                    if (x2 < src.get_x_size()) {
                        r += wc * src.r(x2, y);
                        g += wc * src.g(x2, y);
                        b += wc * src.b(x2, y);
                        n += wc;
                    }
                }
                scratch.r(x, y) = r / n;
                scratch.g(x, y) = g / n;
                scratch.b(x, y) = b / n;
            }
        }

        return nullptr;
    }

    void* blur_vertical(void* arg)
    {
        auto* args = static_cast<BlurArgs*>(arg);
        auto& scratch = *args->scratch;
        auto& dst = *args->dst;
        int radius = args->radius;

        double w[Gauss::max_radius]{};
        Gauss::get_weights(radius, w);

        for (int x = args->start_x; x < args->end_x; x++) {
            for (int y = args->start_y; y < args->end_y; y++) {
                auto r{w[0] * scratch.r(x, y)}, g{w[0] * scratch.g(x, y)}, b{w[0] * scratch.b(x, y)}, n{w[0]};

                for (int wi = 1; wi <= radius; wi++) {
                    auto wc = w[wi];
                    int y2 = y - wi;
                    if (y2 >= 0) {
                        r += wc * scratch.r(x, y2);
                        g += wc * scratch.g(x, y2);
                        b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                    y2 = y + wi;
                    if (y2 < dst.get_y_size()) {
                        r += wc * scratch.r(x, y2);
                        g += wc * scratch.g(x, y2);
                        b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                }
                dst.r(x, y) = r / n;
                dst.g(x, y) = g / n;
                dst.b(x, y) = b / n;
            }
        }

        return nullptr;
    }

    Matrix blur(Matrix m, const int radius, const int num_threads)
    {
        Matrix scratch{PPM::max_dimension};
        Matrix dst{m};

        int cols_per_thread = dst.get_x_size() / num_threads;
        pthread_t threads[num_threads];
        BlurArgs args[num_threads];

        for (int i = 0; i < num_threads; i++) {
            args[i] = {&m, &dst, &scratch, radius, i * cols_per_thread, (i == num_threads - 1) ? static_cast<int>(dst.get_x_size()) : (i + 1) * cols_per_thread, 0, static_cast<int>(dst.get_y_size())};
            pthread_create(&threads[i], nullptr, blur_horizontal, &args[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], nullptr);
        }

        for (int i = 0; i < num_threads; i++) {
            args[i] = {&m, &dst, &scratch, radius, i * cols_per_thread, (i == num_threads - 1) ? static_cast<int>(dst.get_x_size()) : (i + 1) * cols_per_thread, 0, static_cast<int>(dst.get_y_size())};
            pthread_create(&threads[i], nullptr, blur_vertical, &args[i]);
        }

        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], nullptr);
        }

        return dst;
    }
}
