#pragma once

#include <array>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <vector>

#include "raylib.h"

#define UNUSED(...) \
    _Pragma("clang diagnostic push")                                           \
    _Pragma("clang diagnostic ignored \"-Wunused-value\"")                     \
    ((void)(__VA_ARGS__))                                                      \
    _Pragma("clang diagnostic pop")

#define DO_STRINGIFY(X__) #X__
#define STRINGIFY(X__) DO_STRINGIFY(X__)

#define TODO(...) do { \
    TraceLog(LOG_FATAL, "TODO: " __FILE__ ":" STRINGIFY(__LINE__) ": " __VA_ARGS__); \
    exit(-1); \
} while (0)

struct Vector2i {
    int x, y;

    operator Vector2() const noexcept {
        return { (float)x, (float)y };
    }
};

template<typename T>
T min(T a, T b) {
    return a < b ? a : b;
}

template<typename T>
struct Span {
    T* items;
    size_t length;

    Span() = default;
    
    Span(T* items, size_t length) :
        items(items),
        length(length)
    {
    }
    
    template<size_t N>
    Span(std::array<T, N>& arr) :
        items(arr.data()),
        length(arr.size())
    {
    }

    Span(std::vector<T>& vec) :
        items(vec.data()),
        length(vec.size())
    {
    }

    T& operator[](size_t idx) {
        assert(idx < length);
        return items[idx];
    }

    const T& operator[](size_t idx) const {
        assert(idx < length);
        return items[idx];
    }

    T*       begin()  { return items; }
    T*       end()    { return items + length; }
    const T* cbegin() { return items; }
    const T* cend()   { return items + length; }
};

template<typename T>
struct Const_Span {
    const T* items;
    size_t length;

    Const_Span() = default;
    
    Const_Span(const T* items, size_t length) :
        items(items),
        length(length)
    {
    }
    
    template<size_t N>
    Const_Span(const std::array<T, N>& arr) :
        items(arr.data()),
        length(arr.size())
    {
    }

    Const_Span(const std::vector<T>& vec) :
        items(vec.data()),
        length(vec.size())
    {
    }

    const T& operator[](size_t idx) const {
        assert(idx < length);
        return items[idx];
    }

    const T* begin()        { return items; }
    const T* end()          { return items + length; }
    const T* cbegin() const { return items; }
    const T* cend()   const { return items + length; }
};

enum Error {
    ERR_NONE = 0,
};

template<typename T>
struct Result {
    Error err_code;
    T ok_value;

    Result(T ok_value) :
        err_code(ERR_NONE),
        ok_value(std::move(ok_value))
    {
    }

    Result(Error err_code) :
        err_code(err_code)
    {
    }
};
