#pragma once

inline uint32_t swap_endian(uint32_t val) noexcept
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

template<typename T, typename FStream>
inline T read_data_swap_endian(FStream& file)
{
    T temp;
    file.read(reinterpret_cast<char*>(&temp), sizeof(T));
    return static_cast<T>(swap_endian(static_cast<uint32_t>(temp)));
}

template<typename T, typename FStream>
inline T read_data(FStream& file)
{
    T temp;
    file.read(reinterpret_cast<char*>(&temp), sizeof(T));
    return temp;
}

template<typename T, typename FStream>
inline void write_data(T data, FStream& stream)
{
    stream.write(reinterpret_cast<char*>(&data), sizeof(T));
}
