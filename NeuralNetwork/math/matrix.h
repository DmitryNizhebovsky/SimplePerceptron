#pragma once

#include <ostream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <initializer_list>
#include <cassert>
#include <iterator>

#include "..\utils\mat_iterator.h"

namespace ml
{
    namespace math
    {
        template <typename T>
        class matrix
        {
        public:
            using value_type = T;
            using size_type = size_t;
            using difference_type = ptrdiff_t;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;
            using const_reference = const T&;

            using iterator = mat_iterator<T>;
            using reverse_iterator = std::reverse_iterator<iterator>;

            using const_iterator = const_mat_iterator<T>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

            explicit matrix() : sizeM(0), sizeN(0), length(0)
            { }

            explicit matrix(const size_t m, const size_t n)
                : sizeM(m), sizeN(n), length(m * n)
            {
                assert(m != 0 || n != 1 && "invalid matrix sizes");
                assert(m != 1 || n != 0 && "invalid matrix sizes");

                data = std::make_unique<T[]>(length);
                std::fill(data.get(), data.get() + length, static_cast<T>(0));
            }

            explicit matrix(const size_t m, const size_t n, const std::vector<T> values)
                : sizeM(m), sizeN(n), length(m * n)
            {
                assert(m != 0 || n != 1 && "invalid matrix sizes");
                assert(m != 1 || n != 0 && "invalid matrix sizes");

                data = std::make_unique<T[]>(length);
                std::copy(values.cbegin(), values.cend(), data.get());
            }

            explicit matrix(std::initializer_list<std::initializer_list<T>> values)
            {
                size_t index = 0;

                sizeM = values.size();
                sizeN = values.begin()->size();
                length = sizeM * sizeN;

                data = std::make_unique<T[]>(sizeM * sizeN);

                for (const auto& row : values)
                {
                    assert(row.size() == sizeN && "matrix is not set correctly");

                    for (const auto& column : row)
                    {
                        data[index++] = column;
                    }
                }
            }

            matrix(const matrix<T>& m)
            {
                sizeM = m.sizeM;
                sizeN = m.sizeN;
                length = m.length;

                data = std::make_unique<T[]>(length);
                std::copy(m.data.get(), m.data.get() + length, data.get());
            }

            matrix<T>& operator=(const matrix<T>& m)
            {
                if (this != &m)
                {
                    sizeM = m.sizeM;
                    sizeN = m.sizeN;
                    length = m.length;

                    data = std::make_unique<T[]>(length);
                    std::copy(m.data.get(), m.data.get() + length, data.get());
                }

                return *this;
            }

            matrix(matrix<T>&& m)
            {
                sizeM = 0;
                sizeN = 0;
                length = 0;
                data.reset(nullptr);

                std::swap(sizeM, m.sizeM);
                std::swap(sizeN, m.sizeN);
                std::swap(length, m.length);
                std::swap(data, m.data);
            }

            matrix<T>& operator=(matrix<T>&& m)
            {
                if (this != &m)
                {
                    sizeM = 0;
                    sizeN = 0;
                    length = 0;
                    data.reset(nullptr);

                    std::swap(sizeM, m.sizeM);
                    std::swap(sizeN, m.sizeN);
                    std::swap(length, m.length);
                    std::swap(data, m.data);
                }

                return *this;
            }

            size_t size_n() const
            {
                return sizeN;
            }

            size_t size_m() const
            {
                return sizeM;
            }

            size_t size() const
            {
                return length;
            }

            void transpose()
            {
                if ((sizeN == 0 && sizeM == 0) || (sizeN == 1 && sizeM == 1))
                    return;

                if (sizeM == sizeN)
                    transpose_sqr();
                else
                    transpose_rect();
            }

            matrix<T> transposed() const
            {
                matrix<T> copy(*this);
                copy.transpose();
                return copy;
            }

            matrix<T> elem_mul(matrix<T> m1)
            {
                assert(m1.length == length && "matrix sizes are incompatible");

                for (size_t i = 0; i < length; ++i)
                    m1.data[i] *= data[i];

                return m1;
            }

            T& get(size_t i, size_t j)
            {
                assert(i >= 0 && j >= 0 && i < sizeM && j < sizeN && "index out of range");
                return data[i * sizeN + j];
            }

            matrix<T>& operator+=(const matrix<T>& m)
            {
                assert(sizeN == m.sizeN && sizeM == m.sizeM && "matrix sizes are incompatible");

                for (size_t i = 0; i < length; ++i)
                {
                    data[i] += m.data[i];
                }

                return *this;
            }

            matrix<T>& operator-=(const matrix<T>& m)
            {
                assert(sizeN == m.size_n() && sizeM == m.size_m() && "matrix sizes are incompatible");

                for (size_t i = 0; i < length; ++i)
                {
                    data[i] -= m.data[i];
                }

                return *this;
            }

            template<typename Number>
            matrix<T>& operator*=(const Number num)
            {
                for (size_t i = 0; i < length; ++i)
                {
                    data[i] *= static_cast<T>(num);
                }

                return *this;
            }

            template<typename Number>
            matrix<T>& operator/=(const Number num)
            {
                for (size_t i = 0; i < length; ++i)
                {
                    data[i] /= static_cast<T>(num);
                }

                return *this;
            }

            matrix<T>& operator*=(const matrix<T>& m)
            {
                assert(sizeN == m.sizeM && "matrix sizes are incompatible");

                size_t sizeM1 = sizeM;
                size_t sizeN1 = m.sizeN;
                length = sizeM1 * sizeN1;

                auto newData = std::make_unique<T[]>(length);
                std::fill(newData.get(), newData.get() + length, static_cast<T>(0));

                for (size_t i = 0; i < sizeM; ++i)
                {
                    T* c = newData.get() + i * sizeN1;

                    for (size_t k = 0; k < sizeN; ++k)
                    {
                        const T* b = m.data.get() + k * sizeN1;
                        T a = data[i * sizeN + k];

                        for (size_t j = 0; j < sizeN1; ++j)
                            c[j] += a * b[j];
                    }
                }

                std::swap(data, newData);
                sizeN = sizeN1;

                return *this;
            }

            matrix<T> operator+() const
            {
                return *this;
            }

            matrix<T> operator-() const
            {
                matrix<T> temp(sizeM, sizeN);

                for (size_t i = 0; i < length; ++i)
                    temp.data[i] = -data[i];

                return temp;
            }

            template <typename T>
            friend bool operator==(const matrix<T>& m1, const matrix<T>& m2);

            template <typename T>
            friend bool operator!=(const matrix<T>& m1, const matrix<T>& m2);

            template <typename T>
            friend matrix<T> operator*(matrix<T> m1, const matrix<T>& m2);

            template<typename T, typename Number>
            friend matrix<T> operator*(matrix<T> m, const Number n);

            template<typename T, typename Number>
            friend matrix<T> operator*(const Number num, matrix<T> m);

            template<typename T, typename Number>
            friend matrix<T> operator/(matrix<T> m, const Number num);

            template <typename T>
            friend matrix<T> operator+(matrix<T> m1, const matrix<T>& m2);

            template <typename T>
            friend matrix<T> operator-(matrix<T> m1, const matrix<T>& m2);

            template<typename T, typename Number>
            friend matrix<T> operator+(const Number num, const matrix<T>& m1);

            template<typename T, typename Number>
            friend matrix<T> operator-(const Number num, const matrix<T>& m1);

            template<typename T, typename Number>
            friend matrix<T> operator+(const matrix<T>& m1, const Number num);

            template<typename T, typename Number>
            friend matrix<T> operator-(const matrix<T>& m1, const Number num);

            template <typename T>
            friend std::ostream& operator<<(std::ostream& out, const matrix<T>& m);

            iterator begin()
            {
                return iterator(data.get(), length, 0);
            }

            iterator end()
            {
                return iterator(data.get(), length, length);
            }

            reverse_iterator rbegin()
            {
                return reverse_iterator(end());
            }

            reverse_iterator rend()
            {
                return reverse_iterator(begin());
            }

            const_iterator cbegin() const
            {
                return const_iterator(data.get(), length, 0);
            }

            const_iterator cend() const
            {
                return const_iterator(data.get(), length, length);
            }

            const_reverse_iterator crbegin() const
            {
                return const_reverse_iterator(cend());
            }

            const_reverse_iterator crend() const
            {
                return const_reverse_iterator(cbegin());
            }

        private:
            void transpose_sqr()
            {
                for (size_t m = 0; m < sizeM - 1; ++m)
                {
                    for (size_t n = m + 1; n < sizeN; ++n)
                    {
                        std::swap(data[m * sizeN + n], data[n * sizeN + m]);
                    }
                }
            }

            void transpose_rect()
            {
                std::unique_ptr<T[]> newData = std::make_unique<T[]>(sizeM * sizeN);

                for (size_t m = 0; m < sizeM; ++m)
                {
                    for (size_t n = 0; n < sizeN; ++n)
                    {
                        newData[n * sizeM + m] = data[m * sizeN + n];
                    }
                }

                std::swap(sizeM, sizeN);
                std::swap(data, newData);
            }

        private:
            size_t sizeN;
            size_t sizeM;
            size_t length;

            std::unique_ptr<T[]> data;
        };

        template <typename T>
        bool operator==(const matrix<T>& m1, const matrix<T>& m2)
        {
            if (m1.sizeM != m2.sizeM || m1.sizeN != m2.sizeN)
                return false;

            for (size_t i = 0; i < m1.length; ++i)
                if (m1.data[i] != m2.data[i])
                    return false;

            return true;
        }

        template <typename T>
        bool operator!=(const matrix<T>& m1, const matrix<T>& m2)
        {
            return !(m1 == m2);
        }

        template<typename T>
        matrix<T> operator+(matrix<T> m1, const matrix<T>& m2)
        {
            return m1 += m2;
        }

        template<typename T>
        matrix<T> operator-(matrix<T> m1, const matrix<T>& m2)
        {
            return m1 -= m2;
        }

        template<typename T>
        matrix<T> operator*(matrix<T> m1, const matrix<T>& m2)
        {
            return m1 *= m2;
        }

        template<typename T, typename Number>
        matrix<T> operator*(matrix<T> m, const Number num)
        {
            return m *= num;
        }

        template<typename T, typename Number>
        matrix<T> operator*(const Number num, matrix<T> m)
        {
            return m *= num;
        }

        template<typename T, typename Number>
        matrix<T> operator/(matrix<T> m, const Number num)
        {
            return m /= num;
        }

        template<typename T>
        std::ostream& operator<<(std::ostream& out, const matrix<T>& m)
        {
            for (size_t i = 0; i < m.sizeM; ++i)
            {
                for (size_t j = 0; j < m.sizeN; ++j)
                {
                    out << m.data[i * m.sizeN + j] << ' ';
                }

                out << '\n';
            }

            return out;
        }

        template<typename T, typename Number>
        matrix<T> operator+(const Number num, const matrix<T>& m)
        {
            matrix<T> temp(m.sizeM, m.sizeN);
            std::fill(temp.begin(), temp.end(), static_cast<T>(num));

            return temp + m;
        }

        template<typename T, typename Number>
        matrix<T> operator-(const Number num, const matrix<T>& m)
        {
            matrix<T> temp(m.sizeM, m.sizeN);
            std::fill(temp.begin(), temp.end(), static_cast<T>(num));

            return temp - m;
        }

        template<typename T, typename Number>
        matrix<T> operator+(const matrix<T>& m, const Number num)
        {
            return num + m;
        }

        template<typename T, typename Number>
        matrix<T> operator-(const matrix<T>& m, const Number num)
        {
            return -num + m;
        }

        template<typename T>
        matrix<T> elem_mult(matrix<T> m1, const matrix<T>& m2)
        {
            return m1.elem_mul(m2);
        }
    }
}