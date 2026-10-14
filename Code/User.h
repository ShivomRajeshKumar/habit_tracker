#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "Habit.h"

// A user who owns a collection of habits.
class User {
public:
    User() = default;
    explicit User(std::string name) : name_(std::move(name)) {}

    const std::string& name() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    std::vector<Habit>& habits() { return habits_; }
    const std::vector<Habit>& habits() const { return habits_; }

    // Returns false if the name is empty or already exists.
    bool addHabit(std::string habitName) {
        std::replace(habitName.begin(), habitName.end(), '|', '-'); // '|' is our file separator
        if (habitName.empty() || findHabit(habitName)) return false;
        habits_.emplace_back(habitName);
        return true;
    }

    bool removeHabit(size_t index) {
        if (index >= habits_.size()) return false;
        habits_.erase(habits_.begin() + index);
        return true;
    }

    Habit* findHabit(const std::string& habitName) {
        for (auto& h : habits_)
            if (h.name() == habitName) return &h;
        return nullptr;
    }

private:
    std::string name_;
    std::vector<Habit> habits_;
};
