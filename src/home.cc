#include "home.h"
#include "assignment.h"
#include "calculate.h"
#include "interface.h"
#include "settings.h"
#include <format>
#include <vector>

Tab Home() {
  keypad(stdscr, TRUE);
  int button_index_x = 0, button_index_y = 0;
  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++,
                      std::format("Welcome, {} {} (School: {})",
                                  user_settings::student_first_name,
                                  user_settings::student_last_name,
                                  user_settings::school_name),
                      HEADER);
    std::vector<Assignment> assignments;
    assignments = LoadAssignmentsFromDatabase();
    interface.AddText(0, line++, "Your Assignments", SUBHEADER);
    for (size_t i = 0; i < assignments.size() && i < 5; ++i) {
      int style = NORMAL;
      if (i == button_index_y - 1) {
        style |= REVERSE;
      }
      int column = 0;
      interface.AddText(column, line, assignments[i].name, style | BOLD);
      interface.AddText(column += 30, line,
                        std::format("Class: {}", assignments[i].class_name),
                        style);
      interface.AddText(column += 30, line,
                        std::format("Due: {}", assignments[i].due_date), style);
      interface.AddText(
          column += 30, line,
          std::format("Completed: {}", assignments[i].completed ? "Yes" : "No"),
          style);
      interface.AddText(column += 30, line,
                        std::format("Score: {:.2f}/{:.2f}",
                                    assignments[i].score,
                                    assignments[i].max_score),
                        style);
      line++;
    }
    if (assignments.size() > 5) {
      interface.AddText(0, line++,
                        std::format("...and {} more assignments not shown.",
                                    assignments.size() - 5));
    }
    interface.AddText(
        0, line, "[ Add New Assignment ]",
        (button_index_y - 1 == std::min((int)assignments.size(), 5) &&
         button_index_x == 0)
            ? REVERSE
            : NORMAL);
    interface.AddText(
        30, line++, "[ Remove Assignment ]",
        (button_index_y - 1 == std::min((int)assignments.size(), 5) &&
         button_index_x > 0)
            ? REVERSE
            : NORMAL);
    interface.AddText(0, line++, "Grades Summary", SUBHEADER);
    float grade_sum = 0;
    int grade_count = 0;
    for (const auto &assignment : assignments) {
      if (assignment.completed) {
        grade_sum += (assignment.score / assignment.max_score) * 100.0f;
        ++grade_count;
      }
    }
    interface.AddText(
        0, line++,
        std::format(
            "Overall Grade: {:.2f}%, GPA: {:.2f}",
            grade_count > 0 ? static_cast<double>(grade_sum) / grade_count
                            : 0.0,
            CalculateGPA(assignments)));
    interface.AddText(0, line++,
                      std::format("Completed Assignments: {}/{}", grade_count,
                                  static_cast<int>(assignments.size())));
    interface.Draw(0, button_index_y == 0 ? button_index_x : -1);

    auto ch = getch();
    if (ch == '\n') {
      if (button_index_y == 0) {
        if (button_index_x == 1) {
          return Tab::ASSIGNMENTS;
        } else if (button_index_x == 2) {
          return Tab::GRADES;
        } else if (button_index_x == 3) {
          return Tab::CALENDAR;
        } else if (button_index_x == 4) {
          return Tab::SETTINGS;
        }
      } else if (button_index_y - 1 < std::min((int)assignments.size(), 5)) {
        AssignmentMenu(assignments.at(button_index_y - 1));
        SaveAssignmentsToDatabase(assignments);
      } else if (button_index_y - 1 == std::min((int)assignments.size(), 5)) {
        if (button_index_x == 0) {
          auto res = NewAssignment();
          if (!res.canceled) {
            assignments.push_back(res.assignment);
            SaveAssignmentsToDatabase(assignments);
          }
        } else {
          assignments = LoadAssignmentsFromDatabase();
          if (!assignments.empty()) {
            assignments.erase(assignments.begin() + (assignments.size() - 1));
            SaveAssignmentsToDatabase(assignments);
          }
        }
      }
    } else if (ch == 'q' || ch == 'Q') {
      break;
    } else if (ch == KEY_UP) {
      if (button_index_y > 0) {
        --button_index_y;
      }
      button_index_x = 0;
    } else if (ch == KEY_DOWN) {
      if (button_index_y < std::min((int)assignments.size(), 5) + 1) {
        ++button_index_y;
      }
      button_index_x = 0;
    } else if (ch == KEY_LEFT) {
      if (button_index_x > 0) {
        --button_index_x;
      }
    } else if (ch == KEY_RIGHT) {
      if (button_index_x < 4) {
        ++button_index_x;
      }
    }
  }
  return Tab::NONE;
}
