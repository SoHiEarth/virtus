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
    Interface interface;
    interface.AddText(0, 3, std::format("Welcome, {} {} (School: {})", user_settings::student_first_name, user_settings::student_last_name, user_settings::school_name), HEADER);
    std::vector<Assignment> assignments;
    assignments = LoadAssignmentsFromDatabase();
    interface.AddText(0, 4, "Your Assignments", SUBHEADER);
    for (size_t i = 0; i < assignments.size() && i < 5; ++i) {
      int style = NORMAL;
      if (i == button_index_y - 1) {
        style |= REVERSE;
      }
      interface.AddText(0, (int)i + 5, assignments[i].name, style | BOLD);
      interface.AddText(20, (int)i + 5, std::format("Class: {}", assignments[i].class_name), style);
      interface.AddText(40, (int)i + 5, std::format("Due: {}", assignments[i].due_date), style);
      interface.AddText(60, (int)i + 5, std::format("Completed: {}", assignments[i].completed ? "Yes" : "No"), style);
      interface.AddText(80, (int)i + 5, std::format("Score: {:.2f}%/{:.2f}%", assignments[i].score, assignments[i].max_score), style);
    }
    if (assignments.size() > 5) {
      interface.AddText(0, 10, std::format("...and {} more assignments not shown.", assignments.size() - 5));
    }
    interface.AddText(0, 11, "[ Add New Assignment ]", button_index_y - 1 == std::min((int)assignments.size(), 5) ? REVERSE : NORMAL);
    interface.AddText(0, 12, "Grades Summary", SUBHEADER);
    float grade_sum = 0;
    int grade_count = 0;
    for (const auto& assignment : assignments) {
      if (assignment.completed) {
        grade_sum += (assignment.score / assignment.max_score) * 100.0f;
        ++grade_count;
      }
    }
    interface.AddText(0, 13, std::format("Overall Grade: {:.2f}% (Sum: {:.2f}%, Grades counted: {})", grade_count > 0 ? static_cast<double>(grade_sum) / grade_count : 0.0, grade_sum, grade_count));
    interface.AddText(0, 14, std::format("Completed Assignments: {}/{}", grade_count, static_cast<int>(assignments.size())));
    interface.Draw(0, button_index_y == 0 ? button_index_x : -1);

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

