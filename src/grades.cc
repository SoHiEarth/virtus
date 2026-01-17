#include "grades.h"
#include "interface.h"
#include "assignment.h"
#include <map>
#include <format>

Tab Grades() {
  keypad(stdscr, TRUE);
  int button_index_x = 2, button_index_y = 0;
  auto assignments = LoadAssignmentsFromDatabase();
  while (true) {
    Interface interface;
    interface.AddText(0, 3, "Overall", HEADER);
    int grade_sum = 0;
    int grade_count = 0;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        grade_sum += assignment.score;
        ++grade_count;
      }
    }
    interface.AddText(0, 4, std::format("Overall Grade: {:.2f}%", (grade_count != 0) ? (static_cast<double>(grade_sum)/grade_count) : 0));
    interface.AddText(0, 5, std::format("Completed Assignments: {}/{}", grade_count, static_cast<int>(assignments.size())));
    interface.AddText(0, 7, "Grades by Class", SUBHEADER);
    std::map<std::string, std::pair<int, int>> class_grades;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        class_grades[assignment.class_name].first += assignment.score;
        class_grades[assignment.class_name].second += 1;
      }
    }
    int line = 8;
    for (const auto& [class_name, grades] : class_grades) {
      interface.AddText(0, line, class_name);
      interface.AddText(20, line, std::format("{:.2f}%", (grades.second != 0) ? (static_cast<double>(grades.first)/grades.second) : 0));
      line++;
    }
    interface.Draw(2, button_index_y == 0 ? button_index_x : -1);
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

