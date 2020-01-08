#pragma once
#include <iterator>
#include <cassert>

template<typename T>
class mat_iterator
{
public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ptrdiff_t;

    mat_iterator() : data(nullptr), pos(0) { }
    explicit mat_iterator(pointer ptr, size_t size, size_t pos = 0) : data(ptr), size(size), pos(pos) { }
    mat_iterator(const mat_iterator& it) : data(it.data), pos(it.pos), size(it.size) { }
    mat_iterator& operator=(const mat_iterator& it) { if (this != &it) { data = it.data; pos = it.pos; size = it.size; } return *this; }

    mat_iterator& operator++() { ++pos; return *this; }
    mat_iterator& operator--() { --pos; return *this; }
    mat_iterator& operator++(int) { mat_iterator temp(*this); ++pos; return temp; }
    mat_iterator& operator--(int) { mat_iterator temp(*this); --pos; return temp; }

    reference operator[] (const ptrdiff_t n) { verify_offset(n); return *(data + n); }
    reference operator*() const { return *(data + pos); }
    pointer operator->() const { return data + pos; }

    mat_iterator& operator+= (const ptrdiff_t n) { verify_offset(n); pos += n; return *this; }
    mat_iterator& operator-= (const ptrdiff_t n) { verify_offset(-n); pos -= n; return *this; }

    mat_iterator operator+ (const ptrdiff_t n) const { verify_offset(n); mat_iterator temp(*this); return temp += n; }
    mat_iterator operator- (const ptrdiff_t n) const { verify_offset(-n); mat_iterator temp(*this); return temp -= n; }
    ptrdiff_t operator- (const mat_iterator& it) const { return static_cast<ptrdiff_t>(pos - it.pos); }

    bool operator==(const mat_iterator& it) const { assert(data == it.data); return pos == it.pos; }
    bool operator!=(const mat_iterator& it) const { assert(data == it.data); return pos != it.pos; }
    bool operator< (const mat_iterator& it) const { assert(data == it.data); return pos <  it.pos; }
    bool operator> (const mat_iterator& it) const { assert(data == it.data); return pos >  it.pos; }
    bool operator<=(const mat_iterator& it) const { assert(data == it.data); return pos <= it.pos; }
    bool operator>=(const mat_iterator& it) const { assert(data == it.data); return pos >= it.pos; }

private:
    void verify_offset(const ptrdiff_t n) const noexcept
    {
        if (n != 0)
        {
            assert(data && "cannot seek value - initialized array iterator");
        }
        if (n > 0)
        {
            assert((size - n >= static_cast<size_t>(n)) && "cannot seek array iterator after end");
        }
        if (n < 0)
        {
            #pragma warning(suppress: 4146)
            assert((pos >= -static_cast<size_t>(n)) && "cannot seek array iterator before begin");
        }
    }

private:
    size_t pos;
    size_t size;
    pointer data;
};

template<typename T>
mat_iterator<T> operator+ (const ptrdiff_t n, mat_iterator<T> it) { return it += n; }

template<typename T>
class const_mat_iterator
{
public:
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = ptrdiff_t;

    const_mat_iterator() : data(nullptr), pos(0) { }
    explicit const_mat_iterator(pointer ptr, size_t size, size_t pos = 0) : data(ptr), size(size), pos(pos) { }
    const_mat_iterator(const const_mat_iterator& it) : data(it.data), pos(it.pos), size(it.size) { }
    const_mat_iterator& operator=(const const_mat_iterator& it) { if (this != &it) { data = it.data; pos = it.pos; size = it.size; } return *this; }

    const_mat_iterator& operator++() { ++pos; return *this; }
    const_mat_iterator& operator--() { --pos; return *this; }
    const_mat_iterator& operator++(int) { const_mat_iterator temp(*this); ++pos; return temp; }
    const_mat_iterator& operator--(int) { const_mat_iterator temp(*this); --pos; return temp; }

    reference operator[] (const ptrdiff_t n) { verify_offset(n); return *(data + n); }
    reference operator*() const { return *(data + pos); }
    pointer operator->() const { return data + pos; }

    const_mat_iterator& operator+= (const ptrdiff_t n) { verify_offset(n); pos += n; return *this; }
    const_mat_iterator& operator-= (const ptrdiff_t n) { verify_offset(-n); pos -= n; return *this; }

    const_mat_iterator operator+ (const ptrdiff_t n) const { verify_offset(n); const_mat_iterator temp(*this); return temp += n; }
    const_mat_iterator operator- (const ptrdiff_t n) const { verify_offset(-n); const_mat_iterator temp(*this); return temp -= n; }
    ptrdiff_t operator- (const const_mat_iterator& it) const { return static_cast<ptrdiff_t>(pos - it.pos); }

    bool operator==(const const_mat_iterator& it) const { assert(data == it.data); return pos == it.pos; }
    bool operator!=(const const_mat_iterator& it) const { assert(data == it.data); return pos != it.pos; }
    bool operator< (const const_mat_iterator& it) const { assert(data == it.data); return pos <  it.pos; }
    bool operator> (const const_mat_iterator& it) const { assert(data == it.data); return pos >  it.pos; }
    bool operator<=(const const_mat_iterator& it) const { assert(data == it.data); return pos <= it.pos; }
    bool operator>=(const const_mat_iterator& it) const { assert(data == it.data); return pos >= it.pos; }

private:
    void verify_offset(const ptrdiff_t n) const noexcept
    {
        if (n != 0)
        {
            assert(data && "cannot seek value - initialized array iterator");
        }
        if (n > 0)
        {
            assert((size - n >= static_cast<size_t>(n)) && "cannot seek array iterator after end");
        }
        if (n < 0)
        {
            #pragma warning(suppress: 4146)
            assert((pos >= -static_cast<size_t>(n)) && "cannot seek array iterator before begin");
        }
    }

private:
    size_t pos;
    size_t size;
    pointer data;
};

template<typename T>
const_mat_iterator<T> operator+ (const ptrdiff_t n, const_mat_iterator<T> it) { return it += n; }