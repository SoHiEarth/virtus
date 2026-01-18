#include "assignment.h"
#include <iomanip>
#include <sstream>
#include <vector>
#include "string.h"

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
  // If not valid, try again
  while (sscanf(buffer, "%d-%d-%d", new int, new int, new int) != 3) {
    printw("Invalid date format. Please use YYYY-MM-DD.\n");
    refresh();
    getnstr(buffer, 255);
  }
  assignment.due_date = strdup(buffer);
  assignment.completed = false;
  assignment.score = 0;
  assignment.max_score = 100; // Default max score
  printw("Assignment added successfully! Press any key to return to home.");
  refresh();
  getch();
  return assignment;
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

void AssignmentMenu(Assignment *assignment) { AssignmentMenu(*assignment); }

void AssignmentMenu(Assignment &assignment) {
  int button_index_x = 0;
  int button_index_y = 0;
  keypad(stdscr, TRUE);
  while (true) {
    Interface interface;
    interface.AddText(0, 3, assignment.name, HEADER);
    interface.AddText(1, 4, "Assignment Info", SUBHEADER);
    interface.AddText(0, 5, "Name",
                      button_index_x == 0 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, 5, assignment.name,
                      button_index_x == 0 ? REVERSE : NORMAL);
    interface.AddText(0, 6, "Class",
                      button_index_x == 1 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, 6, assignment.class_name,
                      button_index_x == 1 ? REVERSE : NORMAL);
    interface.AddText(0, 7, "Description",
                      button_index_x == 2 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, 7, assignment.description,
                      button_index_x == 2 ? REVERSE : NORMAL);
    interface.AddText(0, 8, "Due Date",
                      button_index_x == 3 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, 8, assignment.due_date,
                      button_index_x == 3 ? REVERSE : NORMAL);
    interface.AddText(0, 9, "Completed",
                      button_index_x == 4 ? REVERSE : NORMAL | BOLD);
    interface.AddText(30, 9, assignment.completed ? "Yes" : "No",
                      button_index_x == 4 ? REVERSE : NORMAL);
    interface.AddText(0, 10, "Score",
                      button_index_x == 5 ? REVERSE : NORMAL | BOLD);
    std::ostringstream score_stream;
    score_stream << std::fixed << std::setprecision(2) << assignment.score
                 << "% / " << assignment.max_score << "%";
    interface.AddText(30, 10, score_stream.str(),
                      button_index_x == 5 ? REVERSE : NORMAL);
    interface.AddText(0, 12, "Actions", SUBHEADER);
    interface.AddText(6, 13, "[ Go Back ]",
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
        while (sscanf(buffer, "%d-%d-%d", new int, new int, new int) != 3) {
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
    for (size_t i = 0; i < assignments.size(); ++i) {
      int style = NORMAL;
      if (i == button_index_y - 1) {
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
                   << "% / " << assignments[i].max_score << "%";
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
      if (button_index_x < 4) {
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
