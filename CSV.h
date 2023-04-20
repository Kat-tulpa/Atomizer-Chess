#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class CSV {
public:
    static constexpr size_t DEFAULT_MAX_LINE_LENGTH = 256;
    CSV(const std::string& filename) : m_file_name(filename), m_file(filename), m_max_line_length(DEFAULT_MAX_LINE_LENGTH) { }

    void setMaxLineLength(const size_t _length) {
        m_max_line_length = _length;
    }

    auto getMaxLineLength() {
        return m_max_line_length;
    }

    auto getNextLineUnparsed() {
        std::string line;
        if (std::getline(m_file, line)) {
            return line;
        }
    }

    std::ifstream& getFile() {
        return m_file;
    }

    std::vector<std::string> readLineDelimited(const std::vector<char>& delimiters, size_t numFields) {
        std::vector<std::string> fields;
        std::string field;

        while (std::getline(m_file, field)) {
            size_t start = 0;
            size_t end = 0;
            size_t fieldCount = 0;

            while (fieldCount < numFields - 1) {
                for (char delimiter : delimiters) {
                    end = field.find_first_of(delimiter, start);
                    if (end != std::string::npos) break;
                }

                if (end == std::string::npos) break;

                fields.push_back(field.substr(start, end - start));
                start = end + 1;
                fieldCount++;
            }

            // Handle the last field separately to avoid adding an extra delimiter
            fields.push_back(field.substr(start));

            if (fields.size() >= numFields) break;
        }

        return fields;
    }

    std::vector<char> getNextLine() {
        std::string line;
        if (std::getline(m_file, line)) {
            return std::vector<char>(line.begin(), line.end());
        }
    }

    void open(const std::string& file_name) {
        m_file.open(file_name);
    }

    void close() {
        m_file.close();
    }

    size_t lineCount() {
        size_t line_count = 0;
        std::string line;
        while (std::getline(m_file, line))
            ++line_count;

        // Cycle the file
        m_file.close();
        m_file.open(m_file_name);

        return line_count;
    }

private:
    std::string m_file_name;
    std::ifstream m_file;
    size_t m_max_line_length;
};