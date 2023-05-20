#pragma once
#include <string>
#include <stdexcept>

class RadixTree {
private:
    struct Node {
        bool is_end;
        size_t value;
        Node* child[13];  // Reduced to 13 for the characters you need
        Node() : is_end(false), value(0) {
            for (int i = 0; i < 13; i++) {
                child[i] = nullptr;
            }
        }
        ~Node() {
            for (auto& c : child)
                delete c;
        }
    };

    Node* root;

public:
    RadixTree() : root(new Node()) {}

    ~RadixTree() { delete root; }

    void insert(const std::string& word, size_t value) {
        Node* curr = root;
        for (char c : word) {
            int idx = get_index(c);
            if (curr->child[idx] == nullptr) {
                curr->child[idx] = new Node();
            }
            curr = curr->child[idx];
        }
        curr->is_end = true;
        curr->value = value;
    }

    std::pair<bool, size_t> search(const std::string& word) const {
        Node* curr = root;
        for (char c : word) {
            int idx = get_index(c);
            if (curr->child[idx] == nullptr) {
                return { false, 0 };
            }
            curr = curr->child[idx];
        }
        return { curr->is_end, curr->value };
    }

private:
    static uint8_t get_index(char c) {
        switch (c) {
        case 'P': return 0;
        case 'p': return 1;
        case 'K': return 2;
        case 'k': return 3;
        case 'N': return 4;
        case 'n': return 5;
        case 'Q': return 6;
        case 'q': return 7;
        case 'R': return 8;
        case 'r': return 9;
        case 'B': return 10;
        case 'b': return 11;
        case ' ': return 12;

        default: throw std::out_of_range("Invalid character");
        }
    }
};