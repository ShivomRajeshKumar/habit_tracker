#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "User.h"
#include "date_utils.h"

// Coordinates logging, reporting, suggestions and saving/loading.
//
// File format (plain text):
//   USER|Alex
//   HABIT|Exercise|2026-10-10,2026-10-11
class Tracker {
public:
    Tracker(User user, std::string filePath)
        : user_(std::move(user)), filePath_(std::move(filePath)) {}

    User& user() { return user_; }
    const User& user() const { return user_; }

    // ---------- Logging ----------
    bool log(size_t habitIndex, const std::string& day) {
        if (habitIndex >= user_.habits().size()) return false;
        user_.habits()[habitIndex].markDone(day);
        return true;
    }

    // ---------- Reporting ----------
    void printReport(int windowDays = 7) const {
        const std::string today = date::today();
        std::cout << "\n=== Habit Report for " << user_.name() << " (" << today << ") ===\n";

        if (user_.habits().empty()) {
            std::cout << "No habits yet. Add one from the menu.\n";
            return;
        }

        std::cout << std::left << std::setw(4) << "#" << std::setw(20) << "Habit"
                  << std::setw(8) << "Streak" << std::setw(7) << "Best"
                  << std::setw(7) << "Total" << std::setw(7) << "Week"
                  << "Last " << windowDays << " days (old -> today)\n";
        std::cout << std::string(75, '-') << '\n';

        for (size_t i = 0; i < user_.habits().size(); ++i) {
            const Habit& h = user_.habits()[i];
            int pct = static_cast<int>(h.completionRate(today, windowDays) * 100);
            std::cout << std::left << std::setw(4) << (i + 1) << std::setw(20) << h.name()
                      << std::setw(8) << h.currentStreak(today)
                      << std::setw(7) << h.longestStreak()
                      << std::setw(7) << h.totalDone()
                      << std::setw(7) << (std::to_string(pct) + "%")
                      << h.weekStrip(today, windowDays) << '\n';
        }
    }

    // Rule-based personalization. This is the hook for ML later:
    // replace the logic with a model that predicts which habit you'll skip.
    void printSuggestions(int windowDays = 7) const {
        const std::string today = date::today();
        std::cout << "\n=== Suggestions ===\n";

        if (user_.habits().empty()) {
            std::cout << "Add a habit first.\n";
            return;
        }

        const Habit* weakest = nullptr;
        double lowest = 2.0;
        for (const auto& h : user_.habits()) {
            double rate = h.completionRate(today, windowDays);
            if (rate < lowest) { lowest = rate; weakest = &h; }

            if (h.currentStreak(today) > 0 && !h.isDone(today))
                std::cout << "- Do \"" << h.name() << "\" today to keep your "
                          << h.currentStreak(today) << "-day streak alive!\n";
        }
        std::cout << "- Needs the most attention: \"" << weakest->name() << "\" ("
                  << static_cast<int>(lowest * 100) << "% in the last "
                  << windowDays << " days).\n";
    }

    // ---------- Persistence ----------
    bool save() const {
        std::ofstream out(filePath_);
        if (!out) return false;
        out << "USER|" << user_.name() << '\n';
        for (const auto& h : user_.habits()) {
            out << "HABIT|" << h.name() << '|';
            bool first = true;
            for (const auto& d : h.dates()) {
                if (!first) out << ',';
                out << d;
                first = false;
            }
            out << '\n';
        }
        return true;
    }

    // Returns false if there's no save file yet.
    bool load() {
        std::ifstream in(filePath_);
        if (!in) return false;

        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back(); // Windows line endings
            std::vector<std::string> parts = split(line, '|');

            if (parts.size() >= 2 && parts[0] == "USER") {
                user_.setName(parts[1]);
            } else if (parts.size() >= 2 && parts[0] == "HABIT") {
                user_.addHabit(parts[1]);
                Habit* h = user_.findHabit(parts[1]);
                if (h && parts.size() >= 3)
                    for (const auto& d : split(parts[2], ','))
                        if (date::isValid(d)) h->markDone(d);
            }
        }
        return true;
    }

private:
    static std::vector<std::string> split(const std::string& s, char sep) {
        std::vector<std::string> out;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, sep)) out.push_back(item);
        return out;
    }

    User user_;
    std::string filePath_;
};
