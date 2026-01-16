#include <vector>
#include <format>
#include "assignment.h"
#include "interface.h"
#include "home.h"
#include "settings.h"

Tab Home() {
  keypad(stdscr, TRUE);
  int button_index_x = 0, button_index_y = 0;
  while (true) {
    clear();
    DrawTabBar({"Home", "Assignments", "Grades", "Settings"}, 0, button_index_y == 0 ? button_index_x : -1);
    move(3, 2);
    SetHeader(std::format("Welcome, {} {} (School: {})\n", user_settings::student_first_name, user_settings::student_last_name, user_settings::school_name).c_str());
    std::vector<Assignment> assignments;
    assignments = LoadAssignmentsFromDatabase();
    move(4, 4);
    SetSubHeader("Your Assignments");
    for (size_t i = 0; i < assignments.size() && i < 5; ++i) {
      if (i == button_index_y - 1) {
        attron(A_REVERSE);
      }
      attron(A_BOLD);
      mvprintw((int)i + 5, 6, "%s", assignments[i].name);
      attroff(A_BOLD);
      mvprintw((int)i + 5, 26, "Class: %s", assignments[i].class_name);
      mvprintw((int)i + 5, 46, "Due: %s", assignments[i].due_date);
      mvprintw((int)i + 5, 66, "Completed: %s", assignments[i].completed ? "Yes" : "No");
      mvprintw((int)i + 5, 81, "Score: %.2f%%/%.2f%%", assignments[i].score, assignments[i].max_score);
      if (i == button_index_y - 1) {
        attroff(A_REVERSE);
      }
    }
    printw("\n");
    if (assignments.size() > 5) {
      mvprintw(10, 6, "...and %d more assignments not shown.\n", static_cast<int>(assignments.size() - 5));
    }
    move(11, 6);
    Button add_assignment_button = {"Add New Assignment", button_index_y - 1 == std::min((int)assignments.size(), 5)};
    add_assignment_button.Draw();

    move(12, 4);
    SetSubHeader("Your Grades");
    float grade_sum = 0;
    int grade_count = 0;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        grade_sum += (assignment.score / assignment.max_score) * 100.0f;
        ++grade_count;
      }
    }
    mvprintw(13, 6, "Overall Grade: %.2f%% (Sum: %.2f%%, Grades counted: %d)\n", grade_count > 0 ? static_cast<double>(grade_sum) / grade_count : 0.0, grade_sum, grade_count);
    mvprintw(14, 6, "Completed Assignments: %d/%d", grade_count, static_cast<int>(assignments.size()));
    refresh();

    auto ch = getch();
    if (ch == '\n') {
      if (button_index_y == 0) {
        if (button_index_x == 1) {
          return Tab::ASSIGNMENTS;
        } else if (button_index_x == 2) {
          return Tab::GRADES;
        } else if (button_index_x == 3) {
          return Tab::SETTINGS;
        }
      } else if (button_index_y - 1 < std::min((int)assignments.size(), 5)) {
        AssignmentMenu(assignments.at(button_index_y - 1));
        SaveAssignmentsToDatabase(assignments);
      } else if (button_index_y - 1 == std::min((int)assignments.size(), 5)) {
        Assignment new_assignment = NewAssignment();
        assignments.push_back(new_assignment);
        SaveAssignmentsToDatabase(assignments);
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
      if (button_index_x < 3) {
        ++button_index_x;
      }
    }
  }
  return Tab::NONE;
}

