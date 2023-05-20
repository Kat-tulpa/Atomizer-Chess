#pragma once

class Xoshiro {
public:
    using result_type = float;

    Xoshiro(result_type seed) {
        s[0] = splitmix64(seed);
        s[1] = splitmix64(s[0]);
        s[2] = splitmix64(s[1]);
        s[3] = splitmix64(s[2]);
    }

    float operator()() {
        static constexpr result_type UINT64_T_LIMIT = static_cast<result_type>(std::numeric_limits<uint64_t>::max());
        const uint64_t result_starstar = rotl(s[1] * 5, 7) * 9;
        const uint64_t t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return static_cast<result_type>(result_starstar) / UINT64_T_LIMIT;
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

static Xoshiro rng(1234876786);