#include "class.h"
#include "assignment.h"
#include "calculate.h"
#include "interface.h"
#include <format>
#include <map>

Tab Classes() {
  keypad(stdscr, TRUE);
  int button_index_x = 2, button_index_y = 0;
  while (true) {
    auto assignments = LoadAssignmentsFromDatabase();
    std::map<std::string, std::vector<Assignment*>> all_class_assignments;
    for (auto& assignment : assignments) {
      all_class_assignments[assignment.class_name].push_back(&assignment);
    }
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, "Your Classes", HEADER);
    for (const auto& [class_name, class_assignments] : all_class_assignments) {
      interface.AddText(0, line++, class_name, SUBHEADER);
      interface.AddText(0, line++, "Grade: " + std::format("{:.2f}", CalculateGPA(assignments, class_name)), NORMAL);
      interface.AddText(0, line++, "GPA: " + std::format("{:.2f}", CalculateGrade(assignments, class_name)), NORMAL);
      if (assignments.empty()) {
        interface.AddText(4, line++, "No assignments found.", NORMAL);
      } else {
        for (const auto& assignment : class_assignments) {
          interface.AddText(4, line++, "- " + assignment->name + " (Due: " + assignment->due_date + ")", NORMAL);
        }
      }
      line++;
    }
    interface.Draw(Tab::CLASSES, button_index_y == 0 ? button_index_x : -1);
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
      if (button_index_x < static_cast<int>(Tab::SETTINGS)) {
        ++button_index_x;
      }
      break;
    case '\n':
      if (button_index_x == static_cast<int>(Tab::HOME)) {
        return Tab::HOME;
      } else if (button_index_x == static_cast<int>(Tab::ASSIGNMENTS)) {
        return Tab::ASSIGNMENTS;
      } else if (button_index_x == static_cast<int>(Tab::GRADES)) {
        return Tab::GRADES;
      } else if (button_index_x == static_cast<int>(Tab::CALENDAR)) {
        return Tab::CALENDAR;
      } else if (button_index_x == static_cast<int>(Tab::SETTINGS)) {
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
