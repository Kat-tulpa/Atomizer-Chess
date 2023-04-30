class Xoshiro256ss {
public:
    using result_type = float;

    Xoshiro256ss(result_type seed) {
        s[0] = splitmix64(seed);
        s[1] = splitmix64(s[0]);
        s[2] = splitmix64(s[1]);
        s[3] = splitmix64(s[2]);
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

        const auto ret_value = static_cast<result_type>(result_starstar) / static_cast<result_type>(std::numeric_limits<uint64_t>::max());
        return ret_value;
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