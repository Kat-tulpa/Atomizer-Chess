#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

class CSV {
private:
    // TODO: Find a way to move this into defs.hpp
    static constexpr size_t DEFAULT_MAX_LINE_LENGTH = 256;

    std::string m_file_name;
    std::ifstream m_file;
    size_t m_max_line_length;
    size_t m_current_line_count;
    size_t m_cutoff_line_count;

public:
    // Below Methods Are Getters -----
    const bool readingCutoff() const {
        return m_current_line_count >= m_cutoff_line_count;
    }

    const bool eof() const {
        return getFileStream().std::ios::eof();
    }

    const std::ifstream& getFileStream() const {
        return m_file;
    }

    auto currentLineCount() const {
        return m_current_line_count;
    }

    auto maxLineLength() const {
        return m_max_line_length;
    }

    // Below Methods Are Setters -----
    // TODO: Unsure if this should be refactored due to if/else branch
    void open(const std::string& file_name) {
        if (!m_file.is_open())
            throw std::ios_base::failure("File not found: " + m_file_name);
        else {
            m_file.open(file_name);
            m_file_name = file_name;
        }
    }

    void close() {
        m_file.close();
        currentLineCount(0);
    }

    void currentLineCount(const size_t current_line_count) {
        m_current_line_count = current_line_count;
    }

    void cutoffLineCount(const size_t cutoff_line_count) {
        m_cutoff_line_count = cutoff_line_count;
    }

    void maxLineLength(const size_t _length) { 
        m_max_line_length = _length; 
    }

    // Below Methods Have Side Effects  -----
    // TODO: Probably refactor this, it's large and messy
    const auto linesInFile() {
        std::string line;
        while (std::getline(m_file, line) && !readingCutoff())
            m_current_line_count++;
        const auto result = m_current_line_count;
        // Cycle the file
        m_file.close();
        m_file.open(m_file_name);
        return result;
    }

    const auto getNextLine() {
        std::string line;
        if (std::getline(m_file, line)) {
            return std::vector<char>(line.begin(), line.end());
        }
    }

    const auto getNextLineUnparsed() {
        std::string line;
        if (std::getline(m_file, line)) {
            return line;
        }
    }

    // TODO: Refactor method, it's way too big
    const auto readLineDelimited(const char* delimiters) {
        std::vector<std::string> fields;
        std::string field;

        if (std::getline(m_file, field)) {
            size_t start = 0;
            size_t end = 0;

            while (true) {
                for (size_t i = 0; i < std::strlen(delimiters); i++) {
                    end = field.find_first_of(delimiters[i], start);
                    if (end != std::string::npos) break;
                }

                if (end == std::string::npos) break;

                fields.push_back(field.substr(start, end - start));
                start = end + 1;
            }

            // Handle the last field separately to avoid adding an extra delimiter
            fields.push_back(field.substr(start));
        }

        return fields;
    }

public:
    CSV(const std::string& filename, const size_t cutoff_line_count) :
        m_file_name(filename),
        m_file(filename),
        m_cutoff_line_count(cutoff_line_count),
        m_max_line_length(DEFAULT_MAX_LINE_LENGTH),
        m_current_line_count(0)
    { 
        open(filename);
    }

    /* Constructor without a cutoff line count will attempt to read
       the entire file to establish a line count. Depending on the
       size of the file, this may take a long, long time to complete */
    CSV(const std::string& filename) :
        m_file_name(filename),
        m_file(filename),
        m_max_line_length(DEFAULT_MAX_LINE_LENGTH),
        m_current_line_count(0)
    {
        open(filename);
        cutoffLineCount(linesInFile());
    }
};