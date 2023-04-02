#include <iostream>
#include "SequenceManager.h"

static constexpr size_t RAND_PIECESEQUENCE_LENGTH = 2;
static unsigned int THREAD_COUNT = std::thread::hardware_concurrency();;

void addRandomSequences(const unsigned int count, const size_t length) {
    const unsigned int countPerThread = count / THREAD_COUNT;
    std::vector<std::thread> threads;
    threads.reserve(THREAD_COUNT);
    for (unsigned int i = 0; i < THREAD_COUNT - 1; i++) {
        threads.emplace_back([&]() {
            for (unsigned int j = 0; j < countPerThread; j++) {
                SequenceManager::add(SequenceManager::makeRandom(length));
            }
            });
    }
    for (unsigned int j = 0; j < countPerThread + count % THREAD_COUNT; j++) {
        SequenceManager::add(SequenceManager::makeRandom(length));
    }
    for (auto& thread : threads) {
        thread.join();
    }
}

void printSequences() {
    std::lock_guard<std::mutex> guard(SequenceManager::map_mutex);
    for (const auto& entry : SequenceManager::sequence_occurances_score) {
        std::cout << entry.first << " : " << std::get<0>(entry.second) << std::endl;
    }
}

int main() {
    addRandomSequences(1000000, RAND_PIECESEQUENCE_LENGTH);
    printSequences();
    return 0;
}