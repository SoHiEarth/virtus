#include "home.h"

#include <format>
#include <utility>
#include <vector>

#include "assignment.h"
#include "calculate.h"
#include "interface.h"
#include "settings.h"

Tab Home() {
  keypad(stdscr, TRUE);
  int button_index_x = 0;
  int button_index_y = 0;
  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++,
                      std::format("Welcome, {} {} (School: {})",
                                  user_settings::student_first_name,
                                  user_settings::student_last_name,
                                  user_settings::school_name),
                      kHeader);
    std::vector<Assignment> assignments;
    assignments = LoadAssignmentsFromDatabase();
    interface.AddText(0, line++, "Your Assignments", kSubheader);
    for (size_t i = 0; i < assignments.size() && i < 5; ++i) {
      int style = kNormal;
      if (std::cmp_equal(i, button_index_y - 1)) {
        style |= kReverse;
      }
      int column = 0;
      interface.AddText(column, line, assignments[i].name, style | kBold);
      interface.AddText(column += 30, line,
                        std::format("Class: {}", assignments[i].class_name),
                        style);
      interface.AddText(column += 30, line,
                        std::format("Due: {}", assignments[i].due_date), style);
      interface.AddText(
          column += 30, line,
          std::format("Completed: {}", assignments[i].completed ? "Yes" : "No"),
          style);
      interface.AddText(
          column += 30, line,
          std::format("Score: {:.2f}/{:.2f}", assignments[i].score,
                      assignments[i].max_score),
          style);
      line++;
    }
    if (assignments.size() > 5) {
      interface.AddText(0, line++,
                        std::format("...and {} more assignments not shown.",
                                    assignments.size() - 5));
    }
    interface.AddText(0, line, "[ Add New Assignment ]",
                      (button_index_y - 1 ==
                           std::min(static_cast<int>(assignments.size()), 5) &&
                       button_index_x == 0)
                          ? kReverse
                          : kNormal);
    interface.AddText(30, line++, "[ Remove Assignment ]",
                      (button_index_y - 1 ==
                           std::min(static_cast<int>(assignments.size()), 5) &&
                       button_index_x > 0)
                          ? kReverse
                          : kNormal);
    interface.AddText(0, line++, "Grades Summary", kSubheader);
    float grade_sum = 0;
    int grade_count = 0;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        grade_sum += (assignment.score / assignment.max_score) * 100.0F;
        ++grade_count;
      }
    }
    interface.AddText(
        0, line++,
        std::format("Overall Grade: {:.2f}%, GPA: {:.2f}",
                    CalculateGrade(assignments), CalculateGPA(assignments)));
    interface.AddText(0, line++,
                      std::format("Completed Assignments: {}/{}", grade_count,
                                  static_cast<int>(assignments.size())));
    interface.Draw(Tab::kHome, button_index_y == 0 ? button_index_x : -1);

    auto ch = getch();
    if (ch == '\n') {
      if (button_index_y == 0) {
        if (button_index_x == static_cast<int>(Tab::kAssignments)) {
          return Tab::kAssignments;
        }
        if (button_index_x == static_cast<int>(Tab::kClasses)) {
          return Tab::kClasses;
        }
        if (button_index_x == static_cast<int>(Tab::kGrades)) {
          return Tab::kGrades;
        }
        if (button_index_x == static_cast<int>(Tab::kCalendar)) {
          return Tab::kCalendar;
        }
        if (button_index_x == static_cast<int>(Tab::kSettings)) {
          return Tab::kSettings;
        }
      } else if (button_index_y - 1 <
                 std::min(static_cast<int>(assignments.size()), 5)) {
        AssignmentMenu(assignments.at(button_index_y - 1));
        SaveAssignmentsToDatabase(assignments);
      } else if (button_index_y - 1 ==
                 std::min(static_cast<int>(assignments.size()), 5)) {
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
      if (button_index_y <
          std::min(static_cast<int>(assignments.size()), 5) + 1) {
        ++button_index_y;
      }
      button_index_x = 0;
    } else if (ch == KEY_LEFT) {
      if (button_index_x > 0) {
        --button_index_x;
      }
    } else if (ch == KEY_RIGHT) {
      if (button_index_x < (static_cast<int>(Tab::kSettings))) {
        ++button_index_x;
      }
    }
  }
  return Tab::kNone;
}
