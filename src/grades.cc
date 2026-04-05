#include "grades.h"

#include <format>
#include <map>

#include "assignment.h"
#include "calculate.h"
#include "interface.h"

Tab Grades() {
  keypad(stdscr, TRUE);
  int button_index_x = 3;
  int button_index_y = 0;
  auto assignments = LoadAssignmentsFromDatabase();
  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, "Overall Grades", kHeader);
    int grade_sum = 0;
    int grade_count = 0;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        grade_sum += assignment.score;
        ++grade_count;
      }
    }
    interface.AddText(
        0, line++,
        std::format("Overall Grade: {:.2f}%",
                    (grade_count != 0)
                        ? (static_cast<double>(grade_sum) / grade_count)
                        : 0));
    interface.AddText(0, line++, "Overall Grade Breakdown by Class",
                      kSubheader);
    std::map<std::string, std::pair<int, int>> class_grades;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        class_grades[assignment.class_name].first += assignment.score;
        class_grades[assignment.class_name].second += 1;
      }
    }
    for (const auto& [class_name, grades] : class_grades) {
      int class_grade_sum = 0;
      int class_grade_count = 0;
      for (const auto& assignment : assignments) {
        if (assignment.class_name == class_name && assignment.completed) {
          class_grade_sum += assignment.score;
          ++class_grade_count;
        }
      }
      interface.AddText(0, line, class_name);
      interface.AddText(
          20, line,
          std::format("{:.2f}%", (static_cast<double>(class_grade_sum) /
                                  class_grade_count)));
      line++;
    }
    if (class_grades.empty()) {
      interface.AddText(2, line++, "No classes found. Add one by setting the \"class\" property in a assignment.", kSubheader);
    }

    interface.AddText(0, line++, "Overall GPA", kHeader);
    interface.AddText(
        0, line++,
        std::format("Overall GPA: {:.2f}", CalculateGPA(assignments)));

    interface.AddText(0, line++, "Overall GPA Breakdown by Class", kSubheader);
    for (const auto& [class_name, grades] : class_grades) {
      interface.AddText(0, line, class_name);
      interface.AddText(
          20, line++,
          std::format("{:.2f}", CalculateGPA(assignments, class_name)));
    }

    std::vector<std::string> classes;
    for (const auto& [class_name, grades] : class_grades) {
      bool found = false;
      for (const auto& name : classes) {
        if (name == class_name) {
          found = true;
          break;
        }
      }
      if (!found) {
        classes.push_back(class_name);
      }
    }
    if (class_grades.empty()) {
      interface.AddText(2, line++, "No classes found. Add one by setting the \"class\" property in a assignment.", kSubheader);
    }

    interface.AddText(0, line++, "Contribution", kHeader);
    for (const auto& class_name : classes) {
      interface.AddText(
          0, line++,
          std::format("Grade Contribution by Assignment for {}", class_name),
          kSubheader);
      int line_start = line;
      for (const auto& assignment : assignments) {
        if (assignment.class_name == class_name && assignment.completed) {
          interface.AddText(0, line,
                            std::format("{}. {}: {}%", line - line_start + 1,
                                        assignment.name, assignment.score));
          line++;
        }
      }
    }
    if (classes.empty()) {
      interface.AddText(0, line++,
                        "No completed assignments to show contribution.",
                        kNormal | kItalic);
    }

    interface.AddText(0, line++, "Information", kHeader);
    interface.AddText(0, line++,
                      std::format("Completed Assignments: {}/{}", grade_count,
                                  static_cast<int>(assignments.size())));

    interface.Draw(Tab::kGrades, button_index_y == 0 ? button_index_x : -1);
    auto ch = getch();
    switch (ch) {
      case KEY_UP:
        if (button_index_y > 0) {
          --button_index_y;
        }
        button_index_x = 0;
        break;
      case KEY_LEFT:
        if (button_index_x > 0) {
          --button_index_x;
        }
        break;
      case KEY_RIGHT:
        if (button_index_x < static_cast<int>(Tab::kSettings)) {
          ++button_index_x;
        }
        break;
      case '\n':
        if (button_index_x == static_cast<int>(Tab::kHome)) {
          return Tab::kHome;
        } else if (button_index_x == static_cast<int>(Tab::kAssignments)) {
          return Tab::kAssignments;
        } else if (button_index_x == static_cast<int>(Tab::kClasses)) {
          return Tab::kClasses;
        } else if (button_index_x == static_cast<int>(Tab::kCalendar)) {
          return Tab::kCalendar;
        } else if (button_index_x == static_cast<int>(Tab::kSettings)) {
          return Tab::kSettings;
        }
        break;
      case 'q':
      case 'Q':
        return Tab::kNone;
      default:
        break;
    }
  }
  return Tab::kNone;
}
