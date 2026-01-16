#include "assignment.h"
#include <vector>

std::vector<Assignment> LoadAssignmentsFromDatabase() {
    std::vector<Assignment> assignments;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("assignments.xml");
    if (!result) {
      return assignments;
    }
    pugi::xml_node assignments_node = doc.child("assignments");
    for (pugi::xml_node assignment_node : assignments_node.children("assignment")) {
        Assignment assignment{};
        assignment.name = strdup(assignment_node.child("name").text().as_string());
        assignment.class_name = strdup(assignment_node.child("class_name").text().as_string());
        assignment.description = strdup(assignment_node.child("description").text().as_string());
        assignment.due_date = strdup(assignment_node.child("due_date").text().as_string());
        assignment.completed = assignment_node.child("completed").text().as_bool();
        assignment.score = assignment_node.child("score").text().as_int();
        assignment.max_score = assignment_node.child("max_score").text().as_int();
        assignments.push_back(assignment);
    }
    return assignments;
}

void SaveAssignmentsToDatabase(const std::vector<Assignment>& assignments) {
    pugi::xml_document doc;
    pugi::xml_node assignments_node = doc.append_child("assignments");
    for (const auto& assignment : assignments) {
        pugi::xml_node assignment_node = assignments_node.append_child("assignment");
        assignment_node.append_child("name").text() = assignment.name;
        assignment_node.append_child("class_name").text() = assignment.class_name;
        assignment_node.append_child("description").text() = assignment.description;
        assignment_node.append_child("due_date").text() = assignment.due_date;
        assignment_node.append_child("completed").text() = assignment.completed;
        assignment_node.append_child("score").text() = assignment.score;
        assignment_node.append_child("max_score").text() = assignment.max_score;
    }
    doc.save_file("assignments.xml");
}

Assignment NewAssignment() {
  clear();
  SetHeader("Add New Assignment");
  Assignment assignment{};
  char buffer[256];
  printw("Enter assignment name: ");
  refresh();
  getnstr(buffer, 255);
  assignment.name = strdup(buffer);
  printw("Enter class name: ");
  refresh();
  getnstr(buffer, 255);
  assignment.class_name = strdup(buffer);
  printw("Enter description: ");
  refresh();
  getnstr(buffer, 255);
  assignment.description = strdup(buffer);
  printw("Enter due date (YYYY-MM-DD): ");
  refresh();
  getnstr(buffer, 255);
  assignment.due_date = strdup(buffer);
  assignment.completed = false;
  assignment.score = 0;
  assignment.max_score = 100; // Default max score
  printw("Assignment added successfully! Press any key to return to home.");
  refresh();
  getch();
  return assignment;
}

void DrawPair(int x, const char* label, const char* value, bool highlight) {
  if (highlight) {
    attron(A_REVERSE);
  }
  mvprintw(x, 6, "%s", label);
  mvprintw(x, 20, "%s\n", value);
  if (highlight) {
    attroff(A_REVERSE);
  }
}

void AssignmentMenu(Assignment& assignment) {
  int button_index = 0;
  keypad(stdscr, TRUE);
  while (true) {
    clear();
    move(0, 2);
    SetHeader(assignment.name);
    move(1, 4);
    SetSubHeader("Assignment Info");
    DrawPair(2, "Name",         assignment.name,                      button_index == 0);
    DrawPair(3, "Class",        assignment.class_name,                button_index == 1);
    DrawPair(4, "Description",  assignment.description,               button_index == 2);
    DrawPair(5, "Due Date",     assignment.due_date,                  button_index == 3);
    DrawPair(6, "Completed",    assignment.completed ? "Yes" : "No",  button_index == 4);
    DrawPair(7, "Score", std::to_string(assignment.score).c_str(),    button_index == 5);
    move(8, 4);
    SetSubHeader("Actions");
    move(9, 6);
    Button return_button = {"Go Back", button_index == 6};
    return_button.Draw();
    auto ch = getch();
    if (ch == KEY_UP) {
      if (button_index > 0) {
        --button_index;
      }
    } else if (ch == KEY_DOWN) {
      if (button_index < 6) {
        ++button_index;
      }
    } else if (ch == '\n') {
      switch (button_index) {
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
    clear();
    DrawTabBar({"Home", "Assignments", "Grades", "Settings"}, 1, button_index_y == 0 ? button_index_x : -1);
    move(3, 2);
    SetHeader("All Assignments");
    for (size_t i = 0; i < assignments.size(); ++i) {
      auto& assignment = assignments[i];
      if (i == button_index_y - 1) {
        attron(A_REVERSE);
      }
      attron(A_BOLD);
      mvprintw(i + 4, 4, "%s", assignment.name);
      attroff(A_BOLD);
      mvprintw(i + 4, 24, "Class: %s", assignment.class_name);
      mvprintw(i + 4, 49, "Due: %s", assignment.due_date);
      mvprintw(i + 4, 74, "Completed: %s", assignment.completed ? "Yes" : "No");
      mvprintw(i + 4, 99, "Score: %.2f%%/%.2f%%", assignment.score, assignment.max_score);
      if (i == button_index_y - 1) {
        attroff(A_REVERSE);
      }
    }
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
        if (button_index_y < static_cast<int>(assignments.size())) {
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
        if (button_index_y == 0) {
          if (button_index_x == 0) {
            return Tab::HOME;
          } else if (button_index_x == 2) {
            return Tab::GRADES;
          } else if (button_index_x == 3) {
            return Tab::SETTINGS;
          }
        } else {
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
