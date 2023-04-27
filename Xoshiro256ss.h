#include <cstdint>
#include <iostream>

#include <iomanip>

class Xoshiro256ss {
public:
    using result_type = uint64_t;

    Xoshiro256ss(result_type seed) {
        s[0] = splitmix64(seed);
        s[1] = splitmix64(s[0]);
        s[2] = splitmix64(s[1]);
        s[3] = splitmix64(s[2]);

        //std::cout << std::fixed << std::setprecision(2) << std::endl;
    }

    result_type operator()() {
        const uint64_t result_starstar = rotl(s[1] * 5, 7) * 9;
        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result_starstar;
    }

    static float randomFloatXoshiroUnsigned(Xoshiro256ss& rng) {
        const uint64_t result = rng();
        const uint32_t upper_bits = static_cast<uint32_t>(result >> 32);
        const uint32_t lower_bits = static_cast<uint32_t>(result);

        // Convert to float in range [0, 1)
        const float float_max = static_cast<float>(0xffffffff);
        const uint64_t uint64_max = 0xffffffffffffffff;
        const float result_float = static_cast<float>(upper_bits * (float_max + 1) + lower_bits);
        const float result2 = result_float / (uint64_max + 1);
        std::cout << "Unsigned random float: " << result2 << std::endl;
        return result;
    }

    static float randomFloatXoshiroSigned(Xoshiro256ss& rng) {
        const uint64_t result = rng();
        const uint32_t upper_bits = static_cast<uint32_t>(result >> 32);
        const uint32_t lower_bits = static_cast<uint32_t>(result);

        // Convert to float in range [-1, 1)
        const float float_max = static_cast<float>(0x7fffffff);
        const float result_float = static_cast<float>(upper_bits * (float_max + 1) + lower_bits);
        const float result2 = result_float / float_max;
        std::cout << "Unsigned signed float: " << result2 << std::endl;
        return result;
    }

private:
    static uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    static uint64_t splitmix64(uint64_t x) {
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9;
        x ^= x >> 27;
        x *= 0x94d049bb133111eb;
        x ^= x >> 31;
        return x;
    }

    uint64_t s[4];
};

