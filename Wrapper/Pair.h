#pragma once

namespace MlWrapper
{
    template<typename T, typename U>
    public ref class Pair
    {
    public:
        Pair() {}

        Pair(T first, U second)
        {
            _first = first;
            _second = second;
        }

        property T First
        {
            public: T get()
            {
                return _first;
            }

            public: void set(T value)
            {
                _first = value;
            }
        }

        property U Second
        {
            public: U get()
            {
                return _second;
            }

            public: void set(U value)
            {
                _second = value;
            }
        }

    private:
        T _first;
        U _second;
    };
}