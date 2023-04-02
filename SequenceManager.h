#include <random>
#include <string_view>

namespace SequenceManager {
    constexpr char notation[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    constexpr size_t differentTypes = sizeof(notation) - 1;

    static std::vector<std::tuple<std::string, unsigned int, int>> sequence_occurances_score;

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
        for (auto& entry : sequence_occurances_score) {
            if (std::get<0>(entry) == sequence) {
                std::get<1>(entry)++;
                return;
            }
        }
        sequence_occurances_score.emplace_back(sequence, 1, 0);
    }
}