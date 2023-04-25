#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <ctime>
#include <immintrin.h>

// Seed the random number generator
static std::random_device rd; // Create a random device to seed the generator
static std::mt19937 gen(rd()); // Create a Mersenne Twister engine with rd as the seed

class Utility {
public:
    static char* stringToCharArray(const std::string& str) {
        char* charArray = new char[str.size() + 1];
        std::copy_n(str.begin(), str.size(), charArray);
        charArray[str.size()] = '\0'; // null-terminate the char array
        return charArray;
    }

    static std::vector<char> stringToVector(const std::string& str) {
        return std::vector<char>(str.begin(), str.end());
    }

    // Parses a signed integer from a std::string that includes "+, -, and #" chess notation to indicate engine evaluation score
    static int parseEvalScore(std::string evalscore) {
        if (evalscore[0] == '#') { // Checkmate in X moves

            if (evalscore[1] == '+') {
                return 9999 - std::stoi(evalscore.substr(2));
            }
            else if (evalscore[1] == '-') {
                return -9999 + std::stoi(evalscore.substr(2));
            }
            else {
                throw std::invalid_argument("Invalid evalscore format");
            }
        }
        else {
            return std::stoi(evalscore); // Regular Score
        }
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

    static const std::vector<float> SIMDRandomFloats(int size, float magnitude) {
        const float lowerBound = -magnitude;
        const float upperBound = magnitude;

        __m256 min = _mm256_set1_ps(lowerBound); // Load lower bound into SIMD register
        __m256 max = _mm256_set1_ps(upperBound); // Load upper bound into SIMD register

        std::uniform_real_distribution<float> dis(-1.0f, 1.0f); // Create a uniform real distribution between -1 and 1

        std::vector<float> output(size);

        // Generate random floats between -magnitude and +magnitude using SIMD instructions
        for (int i = 0; i < size; i += 8)
        {
            __m256 randFloats = _mm256_setr_ps(
                dis(gen), dis(gen), dis(gen), dis(gen),
                dis(gen), dis(gen), dis(gen), dis(gen)
            ); // Generate 8 random floats between -1 and 1

            randFloats = _mm256_mul_ps(randFloats, max); // Scale random floats to range [-magnitude, magnitude]
            _mm256_storeu_ps(&output[i], randFloats); // Store the generated floats in output
        }

        return output;
    }

    static const std::vector<float> SIMDRandomBools(int size, float probability) {
        __m256 zero = _mm256_setzero_ps(); // Load zero into SIMD register
        __m256 one = _mm256_set1_ps(1.0f); // Load one into SIMD register

        std::uniform_real_distribution<float> dis(0.0f, 1.0f); // Create a uniform real distribution between 0 and 1

        std::vector<float> output(size);

        // Generate random floats between 0 and 1 using SIMD instructions and round them to 0.f or 1.f based on the input probability
        for (int i = 0; i < size; i += 8)
        {
            __m256 randFloats = _mm256_setr_ps(
                dis(gen), dis(gen), dis(gen), dis(gen),
                dis(gen), dis(gen), dis(gen), dis(gen)
            ); // Generate 8 random floats between 0 and 1

            __m256 randBools = _mm256_and_ps(one, _mm256_cmp_ps(randFloats, _mm256_set1_ps(probability), _CMP_LT_OQ)); // Generate 8 random booleans based on the input probability

            _mm256_storeu_ps(&output[i], randBools); // Store the generated booleans in output
        }

        return output;
    }

    static const std::vector<float> SIMDMultiply(const std::vector<float>& vec1, const std::vector<float>& vec2) {
        const int size = vec1.size();
        const int vectorSize = 8; // AVX2 operates on 8 floats at a time
        const int numVectors = size / vectorSize;

        std::vector<float> result(size);
        __m256 vec1Reg, vec2Reg, resReg;

        for (int i = 0; i < numVectors; i++) {
            vec1Reg = _mm256_loadu_ps(&vec1[i * vectorSize]); // Load 8 floats from vec1
            vec2Reg = _mm256_loadu_ps(&vec2[i * vectorSize]); // Load 8 floats from vec2

            resReg = _mm256_mul_ps(vec1Reg, vec2Reg); // Multiply vec1Reg and vec2Reg element-wise

            _mm256_storeu_ps(&result[i * vectorSize], resReg); // Store the result in the output vector
        }

        // Process the remaining elements outside of the loop
        for (int i = numVectors * vectorSize; i < size; i++) {
            result[i] = vec1[i] * vec2[i];
        }

        return result;
    }

    static const std::vector<float> SIMDAdd(const std::vector<float>& vec1, const std::vector<float>& vec2) {
        const int size = vec1.size();
        const int vectorSize = 8; // AVX2 operates on 8 floats at a time
        const int numVectors = size / vectorSize;

        std::vector<float> result(size);

        __m256 vec1Reg, vec2Reg, resReg;

        for (int i = 0; i < numVectors; i++) {
            vec1Reg = _mm256_loadu_ps(&vec1[i * vectorSize]); // Load 8 floats from vec1
            vec2Reg = _mm256_loadu_ps(&vec2[i * vectorSize]); // Load 8 floats from vec2

            resReg = _mm256_add_ps(vec1Reg, vec2Reg); // Add vec1Reg and vec2Reg element-wise

            _mm256_storeu_ps(&result[i * vectorSize], resReg); // Store the result in the output vector
        }

        // Process the remaining elements outside of the loop
        for (int i = numVectors * vectorSize; i < size; i++) {
            result[i] = vec1[i] + vec2[i];
        }

        return result;
    }

    static const float SIMDSum(const std::vector<float>& vec) {
        const int vectorSize = 8; // AVX2 operates on 8 floats at a time
        const int numVectors = vec.size() / vectorSize;

        __m256 sumReg = _mm256_setzero_ps();

        for (int i = 0; i < numVectors; i++) {
            __m256 vecReg = _mm256_loadu_ps(&vec[i * vectorSize]); // Load 8 floats from vec

            sumReg = _mm256_add_ps(sumReg, vecReg); // Add vecReg to sumReg
        }

        // Process the remaining elements outside of the loop
        float sum = 0.0f;
        for (int i = numVectors * vectorSize; i < vec.size(); i++) {
            sum += vec[i];
        }

        // Extract the sum from the sumReg vector using two `_mm256_extractf128_ps` and `_mm_cvtss_f32` instructions
        __m128 sumRegLow = _mm256_extractf128_ps(sumReg, 0);
        __m128 sumRegHigh = _mm256_extractf128_ps(sumReg, 1);
        sumRegLow = _mm_add_ps(sumRegLow, sumRegHigh);
        sum = _mm_cvtss_f32(_mm_hadd_ps(sumRegLow, sumRegLow));

        return sum;
    }
};