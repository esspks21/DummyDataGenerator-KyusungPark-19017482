#pragma once
#include <string>
#include <vector>

// Minimal JSON parser for dot-notation access to string arrays and numbers.
// Supports nested key paths like "templates.sample_types" and "ranges.quantity_min".
class SimpleJsonParser {
public:
    explicit SimpleJsonParser(const std::string& jsonContent);
    static SimpleJsonParser fromFile(const std::string& filePath);

    std::vector<std::string> getStringArray(const std::string& path) const;
    double getNumber(const std::string& path, double defaultValue = 0.0) const;
    int    getInt(const std::string& path, int defaultValue = 0) const;

private:
    std::string m_content;

    std::string findValue(const std::string& path) const;
    static std::string trim(const std::string& s);
};
