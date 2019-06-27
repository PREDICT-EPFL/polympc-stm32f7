#ifndef TIMER_HPP
#define TIMER_HPP

#include <stdio.h>
#include <hal.h>

#include <cmath>
#include <vector>
#include <tuple>
#include <numeric>
#include <algorithm>

// addresses of registers
#define DWT_CONTROL ((uint32_t *)0xE0001000)
#define DWT_CYCCNT ((uint32_t *)0xE0001004)
#define DEMCR ((uint32_t *)0xE000EDFC)

class Timer {
    using Scalar = float;
    using time_point = uint32_t;

    time_point _start, _stop;
    std::vector<Scalar> _samples;

public:
    void cycle_counter_reset(void)
    {
        // enable the use DWT
        *DEMCR = *DEMCR | 0x01000000;
        // enable cycle counter
        *DWT_CONTROL = *DWT_CONTROL | 1;
        // Reset cycle counter
        *DWT_CYCCNT = 0;
    }

    uint32_t cycle_counter_get(void)
    {
        // read number of cycles
        return *DWT_CYCCNT;
    }

    time_point get_time()
    {
        return cycle_counter_get();
    }

    void tic()
    {
        _start = get_time();
    }

    void toc()
    {
        _stop = get_time();
        Scalar t = (_stop - _start)/ (STM32_SYSCLK / 1000000);
        _samples.push_back(t);
    }

    void clear()
    {
        _samples.clear();
    }

    const std::vector<Scalar>& samples()
    {
        return _samples;
    }

    Scalar sum()
    {
        return std::accumulate(_samples.begin(), _samples.end(), 0.0);
    }

    Scalar mean()
    {
        if (_samples.size() == 0) {
            return 0;
        }
        return sum() / _samples.size();
    }

    std::tuple<Scalar, Scalar> mean_std()
    {
        Scalar m, s;

        if (_samples.size() == 0) {
            return std::make_tuple(0.0, 0.0);
        }

        m = mean();

        std::vector<Scalar> diff(_samples.size());
        std::transform(_samples.begin(), _samples.end(), diff.begin(),
                       [m](Scalar x) {
            return x - m;
        }
                       );
        Scalar sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        s = sqrt(sq_sum / diff.size());

        return std::make_tuple(m, s);
    }

    void print()
    {
        double m, s;
        std::tie(m, s) = mean_std();
        printf("time: mean %.2f us,  std %.2f us\n", m, s);
    }
};

#endif /* TIMER_HPP */