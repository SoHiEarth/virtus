#include <ncurses.h>
#include <string>
#include <filesystem>
#include <pugixml.hpp>
#include "assignment.h"
#include "interface.h"
#include "settings.h"
#include "grades.h"
#include "home.h"

void InitialSetupPrompt() {
  SetSubHeader("Initial Setup");
  printw("Hello! Let's get started with setting up your profile.\n");
  printw("Please enter your first name: ");
  refresh();
  user_settings::student_first_name.resize(256);
  getnstr(user_settings::student_first_name.data(), 255);
  user_settings::student_last_name.resize(256);
  refresh();
  printw("Please enter your last name: ");
  getnstr(user_settings::student_last_name.data(), 255);
  printw("Please enter your school name: ");
  refresh();
  user_settings::school_name.resize(256);
  getnstr(user_settings::school_name.data(), 255);
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
  Tab current_tab = Tab::HOME;
  while (current_tab != Tab::NONE) {
    switch (current_tab) {
      case Tab::HOME:
        current_tab = Home();
        break;
      case Tab::ASSIGNMENTS:
        current_tab = Assignments();
        break;
      case Tab::GRADES:
        current_tab = Grades();
        break;
      case Tab::SETTINGS:
        current_tab = Settings();
        break;
      default:
        current_tab = Tab::NONE;
        break;
    }
  }
  endwin();
  return 0;
}
