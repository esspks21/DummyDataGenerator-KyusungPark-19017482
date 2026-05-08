#include "SimpleJsonParser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

SimpleJsonParser::SimpleJsonParser(const std::string& jsonContent)
    : m_content(jsonContent) {}

SimpleJsonParser SimpleJsonParser::fromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open template file: " + filePath);
    std::ostringstream ss;
    ss << file.rdbuf();
    return SimpleJsonParser(ss.str());
}

std::string SimpleJsonParser::trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Navigate dot-separated path into nested JSON objects and return raw value string.
std::string SimpleJsonParser::findValue(const std::string& path) const {
    std::vector<std::string> keys;
    {
        std::istringstream ss(path);
        std::string k;
        while (std::getline(ss, k, '.'))
            if (!k.empty()) keys.push_back(k);
    }

    std::string scope = m_content;

    for (size_t ki = 0; ki < keys.size(); ++ki) {
        const std::string searchKey = "\"" + keys[ki] + "\"";
        size_t keyPos = scope.find(searchKey);
        if (keyPos == std::string::npos) return "";

        size_t colonPos = scope.find(':', keyPos + searchKey.size());
        if (colonPos == std::string::npos) return "";

        size_t valStart = colonPos + 1;
        while (valStart < scope.size() && std::isspace((unsigned char)scope[valStart]))
            ++valStart;
        if (valStart >= scope.size()) return "";

        char fc = scope[valStart];

        auto findClose = [&](char open, char close) -> size_t {
            int depth = 1;
            size_t pos = valStart + 1;
            bool inStr = false;
            while (pos < scope.size() && depth > 0) {
                char c = scope[pos];
                if (c == '"' && (pos == 0 || scope[pos - 1] != '\\')) inStr = !inStr;
                if (!inStr) {
                    if (c == open)  ++depth;
                    else if (c == close) --depth;
                }
                ++pos;
            }
            return pos;
        };

        if (fc == '{') {
            size_t end = findClose('{', '}');
            scope = scope.substr(valStart, end - valStart);
        } else if (fc == '[') {
            size_t end = findClose('[', ']');
            if (ki == keys.size() - 1)
                return scope.substr(valStart, end - valStart);
            return "";
        } else {
            // Primitive value
            size_t valEnd = valStart;
            if (fc == '"') {
                valEnd = valStart + 1;
                while (valEnd < scope.size()) {
                    if (scope[valEnd] == '"' && scope[valEnd - 1] != '\\') break;
                    ++valEnd;
                }
                ++valEnd;
            } else {
                while (valEnd < scope.size() &&
                       scope[valEnd] != ',' && scope[valEnd] != '}' &&
                       scope[valEnd] != '\n' && scope[valEnd] != '\r')
                    ++valEnd;
            }
            if (ki == keys.size() - 1)
                return trim(scope.substr(valStart, valEnd - valStart));
            return "";
        }
    }

    return scope;
}

std::vector<std::string> SimpleJsonParser::getStringArray(const std::string& path) const {
    std::string val = trim(findValue(path));
    if (val.empty() || val.front() != '[') return {};

    std::vector<std::string> result;
    size_t pos = 1;
    while (pos < val.size()) {
        while (pos < val.size() && std::isspace((unsigned char)val[pos])) ++pos;
        if (pos >= val.size() || val[pos] == ']') break;

        if (val[pos] == '"') {
            size_t start = pos + 1;
            size_t end   = start;
            while (end < val.size()) {
                if (val[end] == '"' && (end == 0 || val[end - 1] != '\\')) break;
                ++end;
            }
            result.push_back(val.substr(start, end - start));
            pos = end + 1;
        } else if (val[pos] != ',') {
            size_t start = pos;
            while (pos < val.size() && val[pos] != ',' && val[pos] != ']' &&
                   !std::isspace((unsigned char)val[pos]))
                ++pos;
            std::string item = trim(val.substr(start, pos - start));
            if (!item.empty()) result.push_back(item);
        }

        while (pos < val.size() &&
               (std::isspace((unsigned char)val[pos]) || val[pos] == ','))
            ++pos;
    }
    return result;
}

double SimpleJsonParser::getNumber(const std::string& path, double defaultValue) const {
    std::string val = trim(findValue(path));
    if (val.empty()) return defaultValue;
    if (!val.empty() && val.back() == ',') val.pop_back();
    try { return std::stod(val); }
    catch (...) { return defaultValue; }
}

int SimpleJsonParser::getInt(const std::string& path, int defaultValue) const {
    return static_cast<int>(getNumber(path, static_cast<double>(defaultValue)));
}
