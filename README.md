# Habit Tracker (C++) OOPs

A simple console app to track daily habits and streaks, built with object-oriented C++ (Habit, User, Tracker classes).

## Features

- Add and delete habits
- Mark habits done (today or a past date)
- View streaks and weekly progress
- Get simple suggestions
- Saves data to `habits.txt`

## Files

- `main.cpp` - menu and user input
- `Tracker.h` - logging, reports, save/load
- `User.h` - holds the habits
- `Habit.h` - streak and progress logic
- `date_utils.h` - date helpers

## Run

```
g++ -std=c++17 main.cpp -o habit_tracker
./habit_tracker
```

On Windows PowerShell, use `.\habit_tracker` to run.

## Future Work

Add AI-based personalization, such as predicting which habit you're likely to skip.
