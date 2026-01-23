#include "assignment.h"
#include "calendar.h"
#include "grades.h"
#include "home.h"
#include "interface.h"
#include "settings.h"
#include <filesystem>
#include <ncurses.h>
#include <pugixml.hpp>
#include <string>
#include <map>

void InitialSetupPrompt() {
  std::map<std::string, std::string> inputs{
    {"First Name", ""},
    {"Last Name", ""},
    {"School Name", ""}
  };
  int button_index_y = 0;
  keypad(stdscr, TRUE);
  while (true) {
    Interface interface;
    int line = 3;
    interface.AddText(0, line++, "Initial Setup", HEADER);
    interface.AddText(0, line++, "Welcome to Virtus. Let's set up your profile.", SUBHEADER);
    int index = 0;
    for (const auto &input : inputs) {
      int style = NORMAL;
      if (index == button_index_y) {
        style |= REVERSE;
      }
      interface.AddText(0, line++, input.first + ": " + input.second, style);
      index++;
    }
    int style = NORMAL;
    if (button_index_y == index) {
      style |= REVERSE;
    }
    interface.AddText(0, line++, "Finish", style | BOLD);
    interface.Draw(0, -1, true);

    auto ch = getch();
    switch (ch) {
      case KEY_UP:
        if (button_index_y > 0) {
          --button_index_y;
        }
        break;
      case KEY_DOWN:
        if (button_index_y < inputs.size()) {
          ++button_index_y;
        }
        break;
      case '\n':
        if (button_index_y < inputs.size()) {
          echo();
          char input[100];
          move(LINES - 1, 0);
          clrtoeol();
          mvprintw(LINES - 1, 0, "Enter %s: ",
                    std::next(inputs.begin(), button_index_y)->first.c_str());
          getnstr(input, 99);
          noecho();
          std::next(inputs.begin(), button_index_y)->second = std::string(input);
        } else {
          bool all_filled = true;
          for (const auto &input : inputs) {
            if (input.second.empty()) {
              all_filled = false;
              break;
            }
          }
          if (all_filled) {
            user_settings::student_first_name = inputs["First Name"];
            user_settings::student_last_name = inputs["Last Name"];
            user_settings::school_name = inputs["School Name"];
            SaveUserSettings();
            return;
          }
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
    } catch (const std::exception &e) {
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
    case Tab::CALENDAR:
      current_tab = Calendar();
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
