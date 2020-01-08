#pragma once

#include <cmath>

namespace ml
{
    namespace function
    {
        template<typename T>
        T sigmoid_function(T x)
        {
            return static_cast<T>(1.0 / (1.0 + exp(-x)));
        }
    }
}