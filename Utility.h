#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <ctime>
#include <immintrin.h>
#include <thread>

#include "RandomDevice.h"

class Utility {
public:
    static char* stringToCharArray(const std::string& str) {
        char* charArray = new char[str.size() + 1];
        std::copy_n(str.begin(), str.size(), charArray);
        charArray[str.size()] = '\0'; // null-terminate the char array
        return charArray;
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

    /* static float calculateAccuracy(float actual_result, float expected_result, float error_scaling_factor) {
        float error = fabs(actual_result - expected_result);
        if (expected_result != 0.0)
            return 1.0 - (error / expected_result);
        else
            return 1.0 - (error / error_scaling_factor);
    } */
};