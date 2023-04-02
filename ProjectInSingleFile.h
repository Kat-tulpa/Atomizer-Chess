#include <random>
#include <string_view>
#include <unordered_map>
#include <tuple>
#include <mutex>
#include <vector>
#include <bitset>
#include <iostream>
#include <thread>

namespace SequenceManager {
    static std::unordered_map<std::string, std::tuple<unsigned int, int>> sequence_occurances_score;
    static std::mutex mutex;

    template <typename CharType>
    std::basic_string<CharType> makeRandom(size_t length, const CharType* characters, size_t numCharacters) {
        static std::minstd_rand rng(std::random_device{}());
        static std::uniform_int_distribution<size_t> dist(0, numCharacters - 1);

        std::basic_string<CharType> sequence(length, 0);
        for (size_t i = 0; i < length; i++) {
            sequence[i] = characters[dist(rng)];
        }
        return sequence;
    }

    static void add(const std::string& sequence) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = sequence_occurances_score.find(sequence);
        if (it != sequence_occurances_score.end()) {
            std::get<0>(it->second)++;
        }
        else {
            sequence_occurances_score.emplace(sequence, std::make_tuple(1, 0));
        }
    }

    static void addRandomChunk(size_t count, size_t length, const char* characters, size_t numCharacters) {
        for (size_t j = 0; j < count; j++) {
            add(makeRandom(length, characters, numCharacters));
        }
    }

    static void addRandom(const unsigned int count, const size_t length, const char* characters, size_t numCharacters) {
        static constexpr size_t NUM_THREADS = 4;
        static constexpr size_t CHUNK_SIZE = count / NUM_THREADS;

        std::vector<std::thread> threads;
        threads.reserve(NUM_THREADS);

        for (size_t i = 0; i < NUM_THREADS - 1; i++) {
            threads.emplace_back(addRandomChunk, CHUNK_SIZE, length, characters, numCharacters);
        }

        size_t remainingCount = count - (NUM_THREADS - 1) * CHUNK_SIZE;
        addRandomChunk(remainingCount, length, characters, numCharacters);

        for (std::thread& thread : threads) {
            thread.join();
        }
    }
};

static constexpr char pieceNotation[] = "-PNBRQKpnbrqk";
static constexpr size_t pieceTypes = sizeof(pieceNotation) - 1;

// Define the 13 possible chess pieces
enum ChessPiece {
    EMPTY,
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_ROOK,
    WHITE_BISHOP,
    WHITE_PAWN,
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_ROOK,
    BLACK_BISHOP,
    BLACK_PAWN
};

// Use bitpacking to store a sequence of chess pieces
class ChessSequence {
public:
    ChessSequence() = default;

    ChessSequence(size_t length) {
        size_t numPieces = length;
        if (numPieces > 64)
            numPieces = 64;
        for (size_t i = 0; i < numPieces; i++)
            setPiece(i, EMPTY);
    }

    void setPiece(size_t index, ChessPiece piece) {
        size_t shift = index * 4;
        data &= ~(0xFULL << shift);
        data |= static_cast<uint64_t>(piece) << shift;
    }

    ChessPiece getPiece(size_t index) const {
        size_t shift = index * 4;
        return static_cast<ChessPiece>((data >> shift) & 0xFULL);
    }

    std::string toString() {
        std::string result;
        for (size_t i = 0; i < getLength(); i++) {
            result += pieceNotation[getPiece(i)];
        }
        return result;
    }

    size_t getLength() {
        return length;
    }

    void setLength(size_t _length) {
        length = _length;
    }

    void randomize() {
        for (size_t i = 0; i < this->length; i++) {
            ChessPiece piece = static_cast<ChessPiece>(std::rand() % pieceTypes);
            setPiece(i, piece);
        }
    }

private:
    uint64_t data;
    size_t length;
};

namespace ChessSequenceManager {
    // Create a random chess sequence and add it to the sequence manager
    static void addRandom(size_t count, size_t length) {
        for (size_t i = 0; i < count; i++) {
            ChessSequence sequence;
            sequence.setLength(length);
            sequence.randomize();
            std::string sequenceString = sequence.toString();
            SequenceManager::add(sequenceString);
        }
    }
};

static constexpr size_t RAND_PIECESEQUENCE_LENGTH = 64;

void printSequences() {
    for (const auto& entry : SequenceManager::sequence_occurances_score) {
        std::cout << entry.first << " : " << std::get<0>(entry.second) << std::endl;
    }
}

int main() {
    ChessSequenceManager::addRandom(1000, RAND_PIECESEQUENCE_LENGTH);
    printSequences();
    return 0;
}