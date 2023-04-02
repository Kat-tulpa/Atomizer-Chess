#include <random>
#include <string_view>
#include <unordered_map>
#include <tuple>
#include <mutex>
#include <thread>
#include <vector>

namespace SequenceManager {
    constexpr char notation[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr size_t differentTypes = sizeof(notation) - 1;

    // Using a thread-safe container and a mutex to protect access to it
    static std::unordered_map<std::string, std::tuple<unsigned int, int>> sequence_occurances_score;
    static std::mutex map_mutex;

    template <typename CharType = char, const char* Notation = notation>
    std::basic_string<CharType> makeRandom(size_t length) {
        static std::minstd_rand rng(std::random_device{}());
        static std::uniform_int_distribution<size_t> dist(0, differentTypes - 1);

        std::basic_string<CharType> sequence(length, 0);
        for (size_t i = 0; i < length; i++) {
            sequence[i] = static_cast<CharType>(Notation[dist(rng)]);
        }
        return sequence;
    }

    static void add(const std::string& sequence) {
        std::lock_guard<std::mutex> guard(map_mutex);
        auto it = sequence_occurances_score.find(sequence);
        if (it != sequence_occurances_score.end()) {
            std::get<0>(it->second)++;
        }
        else {
            sequence_occurances_score.emplace(sequence, std::make_tuple(1, 0));
        }
    }
}