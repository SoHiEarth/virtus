#include "class.h"

#include <format>
#include <map>

#include "assignment.h"
#include "calculate.h"
#include "interface.h"

Tab Classes() {
  keypad(stdscr, TRUE);
  int button_index_x = 2;
  int button_index_y = 0;
  while (true) {
    auto assignments = LoadAssignmentsFromDatabase();
    std::map<std::string, std::vector<Assignment*>> all_class_assignments;
    for (auto& assignment : assignments) {
      all_class_assignments[assignment.class_name].push_back(&assignment);
    }
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, "Your Classes", kHeader);
    for (const auto& [class_name, class_assignments] : all_class_assignments) {
      interface.AddText(0, line++, class_name, kSubheader);
      interface.AddText(
          0, line++,
          "Grade: " +
              std::format("{:.2f}", CalculateGrade(assignments, class_name)),
          kNormal);
      interface.AddText(
          0, line++,
          "GPA: " +
              std::format("{:.2f}", CalculateGPA(assignments, class_name)),
          kNormal);
      if (assignments.empty()) {
        interface.AddText(4, line++, "No assignments found.", kNormal);
      } else {
        for (const auto& assignment : class_assignments) {
          interface.AddText(
              4, line++,
              "- " + assignment->name + " (Due: " + assignment->due_date + ")",
              kNormal);
        }
      }
      line++;
    }
    if (all_class_assignments.empty()) {
      interface.AddText(0, line++,
                        "No classes found. Add one by setting the \"class\" "
                        "parameter in a assignment.",
                        kSubheader);
    }
    interface.Draw(Tab::kClasses, button_index_y == 0 ? button_index_x : -1);
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
        } else if (button_index_x == static_cast<int>(Tab::kGrades)) {
          return Tab::kGrades;
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
