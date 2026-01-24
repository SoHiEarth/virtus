#include "assignment.h"
#include "interface.h"
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>

std::string GetUserInput(const std::string& prompt) {
  echo();
  char input[256];
  mvprintw(LINES - 1, 0, "%s: ", prompt.c_str());
  getnstr(input, 255);
  noecho();
  move(LINES - 1, 0);
  clrtoeol();
  return std::string(input);
}

std::vector<Assignment> LoadAssignmentsFromDatabase() {
  std::vector<Assignment> assignments;
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("assignments.xml");
  if (!result) {
    return assignments;
  }
  pugi::xml_node assignments_node = doc.child("assignments");
  for (pugi::xml_node assignment_node :
       assignments_node.children("assignment")) {
    Assignment assignment{};
    assignment.name = strdup(assignment_node.child("name").text().as_string());
    assignment.class_name =
        strdup(assignment_node.child("class_name").text().as_string());
    assignment.description =
        strdup(assignment_node.child("description").text().as_string());
    assignment.due_date =
        strdup(assignment_node.child("due_date").text().as_string());
    assignment.completed = assignment_node.child("completed").text().as_bool();
    assignment.score = assignment_node.child("score").text().as_int();
    assignment.max_score = assignment_node.child("max_score").text().as_int();
    assignments.push_back(assignment);
  }
  return assignments;
}

void SaveAssignmentsToDatabase(const std::vector<Assignment> &assignments) {
  pugi::xml_document doc;
  pugi::xml_node assignments_node = doc.append_child("assignments");
  for (const auto &assignment : assignments) {
    pugi::xml_node assignment_node =
        assignments_node.append_child("assignment");
    assignment_node.append_child("name").text() = assignment.name.data();
    assignment_node.append_child("class_name").text() = assignment.class_name.data();
    assignment_node.append_child("description").text() = assignment.description.data();
    assignment_node.append_child("due_date").text() = assignment.due_date.data();
    assignment_node.append_child("completed").text() = assignment.completed;
    assignment_node.append_child("score").text() = assignment.score;
    assignment_node.append_child("max_score").text() = assignment.max_score;
  }
  doc.save_file("assignments.xml");
}

NewAssignmentResult NewAssignment() {
  Assignment assignment{};
  int button_index_y = 0;
  int button_index_x = 0;
  keypad(stdscr, TRUE);
  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, "Add New Assignment", HEADER);
    int name_style = button_index_y == 0 ? REVERSE : NORMAL | BOLD;
    interface.AddText(0, line, "Assignment Name", name_style);
    interface.AddText(30, line++, assignment.name.empty() ? "<Not Set>" : assignment.name, name_style & ~BOLD);
    int class_style = button_index_y == 1 ? REVERSE : NORMAL | BOLD;
    interface.AddText(0, line, "Class Name", class_style);
    interface.AddText(30, line++, assignment.class_name.empty() ? "<Not Set>" : assignment.class_name, class_style & ~BOLD);
    int desc_style = button_index_y == 2 ? REVERSE : NORMAL | BOLD;
    interface.AddText(0, line, "Description", desc_style);
    interface.AddText(30, line++, assignment.description.empty() ? "<Not Set>" : assignment.description, desc_style & ~BOLD);

    int due_style = button_index_y == 3 ? REVERSE : NORMAL | BOLD;
    interface.AddText(0, line, "Due Date (YYYY-MM-DD)", due_style);
    interface.AddText(30, line++, assignment.due_date.empty() ? "<Not Set>" : assignment.due_date, due_style & ~BOLD);

    int completed_style = button_index_y == 4 ? REVERSE : NORMAL | BOLD;
    interface.AddText(0, line, "Completed", completed_style);
    interface.AddText(30, line++, assignment.completed ? "Yes" : "No", completed_style & ~BOLD);

    int score_style = button_index_y == 5 ? REVERSE : NORMAL | BOLD;
    interface.AddText(0, line, "Score", score_style);
    interface.AddText(30, line++, std::to_string(assignment.score), score_style & ~BOLD);

    int max_score_style = button_index_y == 6 ? REVERSE : NORMAL | BOLD;
    interface.AddText(0, line, "Max Score", max_score_style);
    interface.AddText(30, line++, std::to_string(assignment.max_score), max_score_style & ~BOLD);
    interface.AddText(0, line++, "Actions", SUBHEADER);
    int add_style = (button_index_y == 7 && button_index_x == 0) ? REVERSE : NORMAL | BOLD;
    interface.AddText(6, line, "[ Add Assignment ]", add_style);
    int cancel_style = (button_index_y == 7 && button_index_x == 1) ? REVERSE : NORMAL | BOLD;
    interface.AddText(30, line++, "[ Cancel ]", cancel_style);
    interface.Draw(-1, -1, true);
    auto ch = getch();
    if (ch == KEY_UP) {
      if (button_index_y > 0) {
        --button_index_y;
      }
    } else if (ch == KEY_DOWN) {
      if (button_index_y < 7) {
        ++button_index_y;
      }
    } else if (ch == KEY_LEFT) {
      if (button_index_y == 7 && button_index_x > 0) {
        --button_index_x;
      }
    } else if (ch == KEY_RIGHT) {
      if (button_index_y == 7 && button_index_x < 1) {
        ++button_index_x;
      }
    } else if (ch == 'q' || ch == 'Q') {
      return {true, Assignment()};
    } else if (ch == '\n') {
      switch (button_index_y) {
        case 0: { // Assignment Name
          assignment.name = GetUserInput("Enter Assignment Name");
          break;
        }
        case 1: { // Class Name
          assignment.class_name = GetUserInput("Enter Class Name");
          break;
        }
        case 2: { // Description
          assignment.description = GetUserInput("Enter Description");
          break;
        }
        case 3: { // Due Date
          while (true) {
            std::string due_date = GetUserInput("Enter Due Date (YYYY-MM-DD)");
            int year, month, day;
            if (sscanf(due_date.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
              assignment.due_date = due_date;
              break;
            } else {
              mvprintw(LINES - 1, 0, "Invalid date format. Please use YYYY-MM-DD.\n");
              refresh();
              getch();
            }
          }
          break;
        }
        case 4: { // Completed
          assignment.completed = !assignment.completed;
          break;
        }
        case 5: { // Score
          while (true) {
            std::string score_str = GetUserInput("Enter Score");
            try {
              assignment.score = std::stof(score_str);
              break;
            } catch (...) {
              mvprintw(LINES - 1, 0, "Invalid score. Please enter a valid value.\n");
              refresh();
              getch();
            }
          }
          break;
        }
        case 6: { // Max Score
          while (true) {
            std::string max_score_str = GetUserInput("Enter Max Score");
            try {
              assignment.max_score = std::stof(max_score_str);
              break;
            } catch (...) {
              mvprintw(LINES - 1, 0, "Invalid max score. Please enter a valid value.\n");
              refresh();
              getch();
            }
          }
          break;
        }
        case 7: { // Actions
          if (button_index_x == 0) { // Add Assignment
            if (assignment.name.empty() || assignment.class_name.empty() ||
                assignment.due_date.empty()) {
              mvprintw(LINES - 1, 0, "Please fill in all required fields.\n");
              refresh();
              getch();
            } else {  
              return {false, assignment};
            }
          } else { // Cancel
            return {true, Assignment()};
          }
          break;
      }
    }
  }
}
}

void DrawPair(int x, const char *label, const char *value, bool highlight) {
  if (highlight) {
    attron(A_REVERSE);
  }
  mvprintw(x, 6, "%s", label);
  mvprintw(x, 20, "%s\n", value);
  if (highlight) {
    attroff(A_REVERSE);
  }
}

void AssignmentMenu(Assignment *assignment) {
  if (assignment)
    AssignmentMenu(*assignment);
}

void AssignmentMenu(Assignment &assignment) {
  int button_index_x = 0;
  int button_index_y = 0;
  keypad(stdscr, TRUE);
  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, assignment.name, HEADER);
    interface.AddText(1, line++, "Assignment Info", SUBHEADER);
    interface.AddText(0, line, "Name", button_index_x == 0 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, line++, assignment.name, button_index_x == 0 ? REVERSE : NORMAL);
    interface.AddText(0, line, "Class", button_index_x == 1 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, line++, assignment.class_name, button_index_x == 1 ? REVERSE : NORMAL);
    interface.AddText(0, line, "Description", button_index_x == 2 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, line++, assignment.description, button_index_x == 2 ? REVERSE : NORMAL);
    interface.AddText(0, line, "Due Date", button_index_x == 3 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, line++, assignment.due_date, button_index_x == 3 ? REVERSE : NORMAL);
    interface.AddText(0, line, "Completed", button_index_x == 4 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, line++, assignment.completed ? "Yes" : "No", button_index_x == 4 ? REVERSE : NORMAL);
    interface.AddText(0, line, "Score", button_index_x == 5 ? REVERSE : NORMAL | BOLD);
    std::ostringstream score_stream;
    score_stream << std::fixed << std::setprecision(2) << assignment.score
                 << "% / " << assignment.max_score << "%";
    interface.AddText(30, line++, score_stream.str(),
                      button_index_x == 5 ? REVERSE : NORMAL);
    interface.AddText(0, line++, "Actions", SUBHEADER);
    interface.AddText(6, line++, "[ Go Back ]",
                      button_index_x == 6 ? REVERSE : NORMAL);
    interface.Draw(-1, -1, true);

    auto ch = getch();
    if (ch == 'q' || ch == 'Q') {
      return;
    } else if (ch == KEY_UP) {
      if (button_index_x > 0) {
        --button_index_x;
      }
    } else if (ch == KEY_DOWN) {
      if (button_index_x < 6) {
        ++button_index_x;
      }
    } else if (ch == '\n') {
      switch (button_index_x) {
      case 0: { // Name
        printw("Enter new name: ");
        char buffer[256];
        refresh();
        getnstr(buffer, 255);
        assignment.name = strdup(buffer);
        break;
      }
      case 1: { // Class
        printw("Enter new class name: ");
        char buffer[256];
        refresh();
        getnstr(buffer, 255);
        assignment.class_name = strdup(buffer);
        break;
      }
      case 2: { // Description
        printw("Enter new description: ");
        char buffer[256];
        refresh();
        getnstr(buffer, 255);
        assignment.description = strdup(buffer);
        break;
      }
      case 3: { // Due Date
        printw("Enter new due date (YYYY-MM-DD): ");
        char buffer[256];
        refresh();
        getnstr(buffer, 255);
        int year, month, day;
        while (sscanf(buffer, "%d-%d-%d", &year, &month, &day) != 3) {
          printw("Invalid date format. Please use YYYY-MM-DD.\n");
          refresh();
          getnstr(buffer, 255);
        }
        assignment.due_date = strdup(buffer);
        break;
      }
      case 4: { // Completed
        assignment.completed = !assignment.completed;
        break;
      }
      case 5: { // Score
        printw("Enter new score: ");
        char buffer[256];
        refresh();
        getnstr(buffer, 255);
        assignment.score = atof(buffer);
        assignment.score =
            std::clamp(assignment.score, 0.0f, assignment.max_score);
        break;
      }
      case 6: // Go Back
        return;
      }
    }
    refresh();
  }
}

Tab Assignments() {
  keypad(stdscr, TRUE);
  int button_index_x = 1, button_index_y = 0;
  auto assignments = LoadAssignmentsFromDatabase();
  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, "All Assignments", HEADER);
    interface.AddText(0, line++, "Actions", SUBHEADER);
    interface.AddText(6, line, "[ New Assignment ]",
                      (button_index_y == 1 && button_index_x == 0)
                          ? REVERSE
                          : NORMAL);
    interface.AddText(30, line, "[ Remove Assignment ]",
                      (button_index_y == 1 && button_index_x == 1)
                          ? REVERSE
                          : NORMAL);
    interface.AddText(60, line++, "[ Refresh List ]",
                      (button_index_y == 1 && button_index_x == 2)
                          ? REVERSE
                          : NORMAL);
    interface.AddText(0, line++, "Assignments", SUBHEADER);
    for (size_t i = 0; i < assignments.size(); ++i) {
      int style = NORMAL;
      if (i == button_index_y - 2) {
        style = REVERSE;
      }
      int column = 0;
      interface.AddText(column, line, assignments[i].name, style | BOLD);
      interface.AddText(column += 30, line,
                        "Class: " + assignments[i].class_name, style);
      interface.AddText(column += 30, line, "Due: " + assignments[i].due_date,
                        style);
      interface.AddText(
          column += 30, line,
          "Completed: " + std::string(assignments[i].completed ? "Yes" : "No"),
          style);
      std::ostringstream score_stream;
      score_stream << std::fixed << std::setprecision(2) << assignments[i].score
                   << "/" << assignments[i].max_score << "";
      interface.AddText(column += 30, line, "Score: " + score_stream.str(),
                        style);
      line++;
    }
    interface.Draw(1, button_index_y == 0 ? button_index_x : -1);

    auto ch = getch();
    switch (ch) {
    case KEY_UP:
      if (button_index_y > 0) {
        --button_index_y;
      }
      button_index_x = 0;
      break;
    case KEY_DOWN:
      if (button_index_y < static_cast<int>(assignments.size()) + 1) {
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
      if (button_index_y == 0 && button_index_x < 4) {
        ++button_index_x;
      }
      if (button_index_y == 1 && button_index_x < 2) {
        ++button_index_x;
      }
      break;
    case '\n':
      if (button_index_y == 0) {
        if (button_index_x == 0) {
          return Tab::HOME;
        } else if (button_index_x == 2) {
          return Tab::GRADES;
        } else if (button_index_x == 3) {
          return Tab::CALENDAR;
        } else if (button_index_x == 4) {
          return Tab::SETTINGS;
        }
      } else if (button_index_y == 1) {
        if (button_index_x == 0) {
          NewAssignmentResult result = NewAssignment();
          if (!result.canceled) {
            assignments.push_back(result.assignment);
            SaveAssignmentsToDatabase(assignments);
          }
        } else if (button_index_x == 1) {
          if (!assignments.empty()) {
            assignments.erase(assignments.begin() + (assignments.size() - 1));
            SaveAssignmentsToDatabase(assignments);
          }
        } else if (button_index_x == 2) {
          assignments = LoadAssignmentsFromDatabase();
        }
      } else if (button_index_y - 2 < static_cast<int>(assignments.size())) {
        AssignmentMenu(assignments[button_index_y - 1]);
        SaveAssignmentsToDatabase(assignments);
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
