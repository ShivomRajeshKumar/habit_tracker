#pragma once

#include <algorithm>
#include <set>
#include <string>

#include "date_utils.h"

// A single habit and the dates on which it was completed.
class Habit {
public:
    explicit Habit(std::string name) : name_(std::move(name)) {}

    const std::string& name() const { return name_; }
    const std::set<std::string>& dates() const { return done_; }

    void markDone(const std::string& day) { done_.insert(day); }
    bool isDone(const std::string& day) const { return done_.count(day) > 0; }
    int totalDone() const { return static_cast<int>(done_.size()); }

    // Consecutive days completed, ending today
    // (or yesterday, so the streak isn't lost before you've done it today).
    int currentStreak(const std::string& today) const {
        std::string day = isDone(today) ? today : date::shift(today, -1);
        int streak = 0;
        while (isDone(day)) {
            ++streak;
            day = date::shift(day, -1);
        }
        return streak;
    }

    int longestStreak() const {
        int best = 0, run = 0;
        std::string prev;
        for (const auto& d : done_) { // std::set keeps ISO dates sorted
            run = (!prev.empty() && date::shift(prev, 1) == d) ? run + 1 : 1;
            best = std::max(best, run);
            prev = d;
        }
        return best;
    }

    // Fraction (0.0 - 1.0) of the last `days` days that were completed.
    double completionRate(const std::string& today, int days) const {
        int count = 0;
        for (int i = 0; i < days; ++i)
            if (isDone(date::shift(today, -i))) ++count;
        return static_cast<double>(count) / days;
    }

    // Visual strip for the last `days` days, oldest to newest, e.g. "X X . X . X X"
    std::string weekStrip(const std::string& today, int days) const {
        std::string s;
        for (int i = days - 1; i >= 0; --i) {
            s += isDone(date::shift(today, -i)) ? "X" : ".";
            if (i > 0) s += ' ';
        }
        return s;
    }

private:
    std::string name_;
    std::set<std::string> done_;
};
