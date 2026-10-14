// Habit Tracker - interactive console app
// Build: g++ -std=c++17 main.cpp -o habit_tracker
// Run:   .\habit_tracker   (Windows)   or   ./habit_tracker   (Linux/Mac)

#include <iostream>
#include <string>

#include "Tracker.h"

// ---------- Input helpers ----------
static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    if (!std::getline(std::cin, s)) { // Ctrl+Z / Ctrl+D or end of input
        std::cout << '\n';
        return "0";
    }
    // trim whitespace
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return a == std::string::npos ? "" : s.substr(a, b - a + 1);
}

// Returns -1 if the input isn't a valid whole number.
static int readInt(const std::string& prompt) {
    std::string s = readLine(prompt);
    try {
        size_t pos = 0;
        int v = std::stoi(s, &pos);
        return pos == s.size() ? v : -1;
    } catch (...) {
        return -1;
    }
}

// Shows habits and lets the user pick one. Returns the 0-based index, or -1.
static int pickHabit(const Tracker& tracker) {
    const auto& habits = tracker.user().habits();
    if (habits.empty()) {
        std::cout << "No habits yet. Add one first.\n";
        return -1;
    }
    std::cout << "\nYour habits:\n";
    for (size_t i = 0; i < habits.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << habits[i].name() << '\n';

    int choice = readInt("Choose a habit number: ");
    if (choice < 1 || choice > static_cast<int>(habits.size())) {
        std::cout << "Invalid choice.\n";
        return -1;
    }
    return choice - 1;
}

static void printMenu() {
    std::cout << "\n===== HABIT TRACKER =====\n"
              << "1. Add a habit\n"
              << "2. Mark a habit done today\n"
              << "3. Mark a habit done on a past date\n"
              << "4. View report\n"
              << "5. Delete a habit\n"
              << "6. Get suggestions\n"
              << "0. Save and exit\n";
}

// ---------- Menu actions ----------
static void addHabit(Tracker& tracker) {
    std::string name = readLine("Habit name: ");
    if (tracker.user().addHabit(name))
        std::cout << "Added \"" << name << "\".\n";
    else
        std::cout << "Could not add habit (empty or already exists).\n";
}

static void markToday(Tracker& tracker) {
    int idx = pickHabit(tracker);
    if (idx < 0) return;
    tracker.log(idx, date::today());
    std::cout << "Logged \"" << tracker.user().habits()[idx].name() << "\" for today. Nice work!\n";
}

static void markPastDate(Tracker& tracker) {
    int idx = pickHabit(tracker);
    if (idx < 0) return;

    std::string day = readLine("Date (YYYY-MM-DD): ");
    if (!date::isValid(day)) {
        std::cout << "Invalid date. Use the format YYYY-MM-DD, e.g. 2026-10-12.\n";
        return;
    }
    if (day > date::today()) { // ISO strings compare chronologically
        std::cout << "You can't log a habit in the future.\n";
        return;
    }
    tracker.log(idx, day);
    std::cout << "Logged \"" << tracker.user().habits()[idx].name() << "\" for " << day << ".\n";
}

static void deleteHabit(Tracker& tracker) {
    int idx = pickHabit(tracker);
    if (idx < 0) return;
    std::string name = tracker.user().habits()[idx].name();
    std::string confirm = readLine("Delete \"" + name + "\"? (y/n): ");
    if (confirm == "y" || confirm == "Y") {
        tracker.user().removeHabit(idx);
        std::cout << "Deleted.\n";
    } else {
        std::cout << "Cancelled.\n";
    }
}

// ---------- Main ----------
int main() {
    Tracker tracker(User(), "habits.txt");

    if (tracker.load()) {
        std::cout << "Welcome back, " << tracker.user().name() << "!\n";
    } else {
        std::cout << "Welcome to Habit Tracker!\n";
        std::string name = readLine("What's your name? ");
        tracker.user().setName(name.empty() ? "Friend" : name);
    }

    while (true) {
        printMenu();
        int choice = readInt("Choose an option: ");

        switch (choice) {
            case 1: addHabit(tracker); break;
            case 2: markToday(tracker); break;
            case 3: markPastDate(tracker); break;
            case 4: tracker.printReport(); break;
            case 5: deleteHabit(tracker); break;
            case 6: tracker.printSuggestions(); break;
            case 0:
                if (tracker.save()) std::cout << "Progress saved to habits.txt. Goodbye!\n";
                else std::cout << "Error: could not save your data.\n";
                return 0;
            default:
                std::cout << "Invalid option. Please enter a number from the menu.\n";
        }
    }
}
