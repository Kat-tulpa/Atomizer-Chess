#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <ctime>
#include <immintrin.h>
#include <thread>
#include <stdexcept>

#include "Xoshiro.hpp"

namespace Utility {
    static char* stringToCharArray(const std::string& str) {
        char* charArray = new char[str.size() + 1];
        std::copy_n(str.begin(), str.size(), charArray);
        charArray[str.size()] = '\0'; // null-terminate the char array
        return charArray;
    }

    static const std::string charVectorToString(const std::vector<char>& char_vector) {
        return std::string(char_vector.begin(), char_vector.end());
    }

    static std::vector<char> stringToCharVector(const std::string& str) {
        return std::vector<char>(str.begin(), str.end());
    }

    static bool isAllThisChar(const std::vector<char>& vec, const char c) {
        for (char v : vec) {
            if (v != c)
                return false;
        }
        return true;
    }

    template <typename T>
    static void shuffleVector(std::vector<T>& vec) {

        // Shuffle the vector using std::rand
        for (size_t i = vec.size() - 1; i > 0; --i) {
            size_t j = std::rand() % (i + 1);
            std::swap(vec[i], vec[j]);
        }
    }

    static float toUnsignedFloat(uint64_t v) {
        return static_cast<uint32_t>(v >> 40) * 0x1.0p-24f;
    }

    static float toSignedFloat(uint64_t v) {
        return (static_cast<uint32_t>(v >> 40) * 0x1.0p-24f) * 2 - 1;
    }

    static int roundUpToNearestMultipleOf8(int n) {
        return ((n + 7) / 8) * 8;
    }

    static size_t charToDigit(const char& c) {
        if (c < '0' || c > '9')
            throw std::invalid_argument("Character was not a digit in range [0, 9]");
        return c - '0';
    }

    static char toChar(const size_t d) {
        if (d < 0 || d > 9)
            throw std::invalid_argument("Digit was not in range [0, 9]");
        return d + '0';
    }
};