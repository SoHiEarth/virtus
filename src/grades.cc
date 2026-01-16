#include "grades.h"
#include "interface.h"
#include "assignment.h"

Tab Grades() {
  keypad(stdscr, TRUE);
  int button_index_x = 2, button_index_y = 0;
  auto assignments = LoadAssignmentsFromDatabase();
  while (true) {
    clear();
    DrawTabBar({"Home", "Assignments", "Grades", "Settings"}, 2, button_index_y == 0 ? button_index_x : -1);
    move(3, 2);
    SetHeader("Overall");
    int grade_sum = 0;
    int grade_count = 0;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        grade_sum += assignment.score;
        ++grade_count;
      }
    }
    mvprintw(4, 4, "Overall Grade: %.2f%% (Sum: %d, Grades counted: %d)", (grade_count != 0) ? (static_cast<double>(grade_sum)/grade_count) : 0, grade_sum, grade_count);
    mvprintw(5, 4, "Completed Assignments: %d/%d", grade_count, static_cast<int>(assignments.size()));
    refresh();
    auto ch = getch();
    switch (ch) {
      case KEY_UP:
        if (button_index_y > 0) {
          --button_index_y;
        }
        button_index_x = 0;
        break;
      case KEY_DOWN:
        if (button_index_y < 1) {
          ++button_index_y;
        }
        button_index_x = 0;
        break;
      case KEY_LEFT:
        if (button_index_x > 0) {
          --button_index_x;
        }
        break;
      case KEY_RIGHT:
        if (button_index_x < 3) {
          ++button_index_x;
        }
        break;
      case '\n':
        if (button_index_x == 0) {
          return Tab::HOME;
        } else if (button_index_x == 1) {
          return Tab::ASSIGNMENTS;
        } else if (button_index_x == 3) {
          return Tab::SETTINGS;
        }
        break;
      case 'q':
      case 'Q':
        return Tab::NONE;
      default:
        break;
    }
  }
  return Tab::NONE;
}

