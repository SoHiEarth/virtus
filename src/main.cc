#include <ncurses.h>
#include <vector>
#include <string>
#include <filesystem>
#include <pugixml.hpp>

constexpr int BUF_SIZE = 256;

struct Assignment {
    int id;
    const char* name;
    const char* class_name;
    const char* description;
    const char* due_date;
    bool completed;
    int score;
    int max_score;
};

struct Button {
    const char* label;
    bool focused;
    void Draw() {
        if (focused) {
            attron(A_REVERSE);
        }
        printw("[ %s ]", label);
        if (focused) {
            attroff(A_REVERSE);
        }
    }
};

namespace user_settings {
  std::string school_name = "Default School";
  std::string student_first_name = "John";
  std::string student_last_name = "Doe";
}

void LoadUserSettings() {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("user_settings.xml");
    if (!result) {
      throw std::runtime_error("Failed to load user_settings.xml");
    }
    pugi::xml_node settings = doc.child("settings");
    user_settings::school_name = settings.child("school_name").text().as_string();
    user_settings::student_first_name = settings.child("student_first_name").text().as_string();
    user_settings::student_last_name = settings.child("student_last_name").text().as_string();
}

void SaveUserSettings() {
    pugi::xml_document doc;
    pugi::xml_node settings = doc.append_child("settings");
    settings.append_child("school_name").text() = user_settings::school_name.c_str();
    settings.append_child("student_first_name").text() = user_settings::student_first_name.c_str();
    settings.append_child("student_last_name").text() = user_settings::student_last_name.c_str();
    doc.save_file("user_settings.xml");
}

std::vector<Assignment> LoadAssignmentsFromDatabase() {
    std::vector<Assignment> assignments;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("assignments.xml");
    if (!result) {
      throw std::runtime_error("Failed to load assignments.xml");
    }
    pugi::xml_node assignments_node = doc.child("assignments");
    for (pugi::xml_node assignment_node : assignments_node.children("assignment")) {
        Assignment assignment;
        assignment.id = assignment_node.attribute("id").as_int();
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
        assignment_node.append_attribute("id") = assignment.id;
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

void SetHeader(const char* title) {
    attron(A_BOLD);
    printw("%s\n", title);
    int width = getmaxx(stdscr);
    for (int i = 0; i < width; ++i) {
        printw("-");
    }
    attroff(A_BOLD);
}

void InitialSetupPrompt() {
  attron(A_UNDERLINE);
  printw("Begin Initial Setup\n");
  attroff(A_UNDERLINE);
  printw("Hello! Let's get started with setting up your profile.\n");
  printw("Please enter your first name: ");
  refresh();
  user_settings::student_first_name.resize(BUF_SIZE);
  getnstr(user_settings::student_first_name.data(), BUF_SIZE - 1);
  user_settings::student_last_name.resize(BUF_SIZE);
  refresh();
  printw("Please enter your last name: ");
  getnstr(user_settings::student_last_name.data(), BUF_SIZE - 1);
  printw("Please enter your school name: ");
  refresh();
  user_settings::school_name.resize(BUF_SIZE);
  getnstr(user_settings::school_name.data(), BUF_SIZE - 1);
  while (user_settings::student_first_name.empty() || user_settings::student_last_name.empty() || user_settings::school_name.empty()) {
      printw("All fields are required. Please try again.\n");
      printw("First name: ");
      refresh();
      getstr(user_settings::student_first_name.data());
      printw("Last name: ");
      refresh();
      getstr(user_settings::student_last_name.data());
      printw("School name: ");
      refresh();
      getstr(user_settings::school_name.data());
  }
  SaveUserSettings();
  printw("Setup complete! Press any key to exit.");
  refresh();
}

Assignment NewAssignment() {
  Assignment assignment;
  char buffer[BUF_SIZE];
  printw("Enter assignment name: ");
  refresh();
  getnstr(buffer, BUF_SIZE - 1);
  assignment.name = strdup(buffer);
  printw("Enter class name: ");
  refresh();
  getnstr(buffer, BUF_SIZE - 1);
  assignment.class_name = strdup(buffer);
  printw("Enter description: ");
  refresh();
  getnstr(buffer, BUF_SIZE - 1);
  assignment.description = strdup(buffer);
  printw("Enter due date (YYYY-MM-DD): ");
  refresh();
  getnstr(buffer, BUF_SIZE - 1);
  assignment.due_date = strdup(buffer);
  assignment.completed = false;
  assignment.score = 0;
  assignment.max_score = 100; // Default max score
  printw("Assignment added successfully! Press any key to return to home.");
  refresh();
  getch();
  return assignment;
}

void Home() {
  keypad(stdscr, TRUE);
  int button_index = 0;
  while (true) {
    clear();
    SetHeader("Home");
    printw("Welcome, %s %s (School: %s)\n", user_settings::student_first_name.c_str(), user_settings::student_last_name.c_str(), user_settings::school_name.c_str());
    std::vector<Assignment> assignments;
    try {
      assignments = LoadAssignmentsFromDatabase();
    } catch (const std::exception& e) {
      printw("Error loading assignments: %s\n", e.what());
    }
    for (size_t i = 0; i < assignments.size(); ++i) {
      printw("Assignment: %s | Class: %s | Due: %s | Completed: %s | Score: %d/%d\t",
        assignments[i].name,
        assignments[i].class_name,
        assignments[i].due_date,
        assignments[i].completed ? "Yes" : "No",
        assignments[i].score,
        assignments[i].max_score);
      if (!assignments[i].completed) {
        Button complete_assignment_button = {"Mark as Completed", (i == button_index)};
        complete_assignment_button.Draw();
      } else {
        Button incomplete_assignment_button = {"Mark as Incomplete", (i == button_index)};
        incomplete_assignment_button.Draw();
      }
      printw("\n");
    }
    Button add_assignment_button = {"Add New Assignment", (button_index == assignments.size())};
    add_assignment_button.Draw();
    refresh();
    auto ch = getch();
    if (ch == '\n') {
      if (button_index < assignments.size()) {
        assignments[button_index].completed = !assignments[button_index].completed;
        SaveAssignmentsToDatabase(assignments);
      } else if (button_index == assignments.size()) {
        Assignment new_assignment = NewAssignment();
        assignments.push_back(new_assignment);
        SaveAssignmentsToDatabase(assignments);
      }
    } else if (ch == 'q' || ch == 'Q') {
      break;
    } else if (ch == KEY_UP) {
      if (button_index > 0) {
        --button_index;
      }
    } else if (ch == KEY_DOWN) {
      if (button_index < assignments.size()) {
        ++button_index;
      }
    }
  }
}

int main() {
  initscr();
  clear();
  if (!std::filesystem::exists("user_settings.xml")) {
    SetHeader("Student Assignment Manager - Initial Setup");
    InitialSetupPrompt();
  } else {
    try {
      LoadUserSettings();
    } catch (const std::exception& e) {
      printw("Error loading user settings: %s\n", e.what());
      printw("Starting initial setup...\n");
      refresh();
      getch();
      InitialSetupPrompt();
    }
  }
  Home();
  endwin();
  return 0;
}
