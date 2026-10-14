#pragma once
// Date helpers. Dates are stored as "YYYY-MM-DD" strings, which sort chronologically.

#include <cctype>
#include <ctime>
#include <string>

namespace date {

inline std::string format(const std::tm& t) {
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
    return buf;
}

inline std::string today() {
    std::time_t now = std::time(nullptr);
    return format(*std::localtime(&now));
}

// Shift a date by n days (n may be negative).
inline std::string shift(const std::string& d, int n) {
    std::tm t{};
    t.tm_year = std::stoi(d.substr(0, 4)) - 1900;
    t.tm_mon = std::stoi(d.substr(5, 2)) - 1;
    t.tm_mday = std::stoi(d.substr(8, 2)) + n;
    t.tm_hour = 12;  // avoids daylight-saving edge cases
    std::mktime(&t); // normalizes the date
    return format(t);
}

// True if d is a real calendar date in YYYY-MM-DD format.
inline bool isValid(const std::string& d) {
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (size_t i = 0; i < d.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(d[i]))) return false;
    }
    if (std::stoi(d.substr(0, 4)) < 1970) return false;
    return shift(d, 0) == d; // rejects things like 2026-02-31
}

} // namespace date
