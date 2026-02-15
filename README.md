# Virtus
![Virtus Banner](assets/logo_banner.png)
## Overview
Virtus is a school assignment manager and grade tracker, helping you manage your schoolwork directly from the terminal - a native habitat per se for developers.

## Features
- Manage school assignments with due dates and priorities.
- Track grades and calculate GPA.
- User-friendly interface for easy navigation.
- Student-friendly functions
- Very light to run, optimized for slow school computers (I know, it's frustrating!😡)

## Preview
### Home
![Home Preview](assets/preview_home.png)
**A glancable, function-oriented home interface full of data and intuitive to use.**
### Assignments
![Assignments Preview](assets/preview_assignment.png)
**A simple interface full of knobs and dials.**
### Classes
![Classes Preview](assets/preview_class.png)
**Automatic class sorting and grades calculation based on the class' name. **
### Grades
![Grades Preview](assets/preview_grades.png)
**A detailed view containing a breakdown of overall/per-class GPA by class or assignment - a novel yet useful feature.**
### Finally, Calendar
![Calendar Preview](assets/preview_calendar.png)
**A simple calendar that lets you know of your recent assignments and their proximity to the current date.**

## Installation
### Download & Run
1. Download the latest release from the [releases page](https://github.com/sohiearth/virtus/releases).
2. Add permissions to execute the file:
   ```bash
   chmod +x virtus
   ```
3. Run the application:
    ```bash
    ./virtus
    ```

### Build from Source
1. Clone the repository:
    ```bash
    git clone https://github.com/sohiearth/virtus.git
    cd virtus
    ```
2. Build the app using CMake:
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```
3. Run the application:
    ```bash
    ./virtus
    ```
