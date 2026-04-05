#include "onboarding.h"

#include <ncurses.h>

#include <map>
#include <stdexcept>
#include <string>

#include "interface.h"
#include "settings.h"

void onboard::Setup() {
  std::map<std::string, std::string> inputs{
      {"First Name", ""}, {"Last Name", ""}, {"School Name", ""}};
  int button_index_y = 0;
  keypad(stdscr, TRUE);
  while (true) {
    Interface interface;
    int line = 3;
    interface.AddText(0, line++, "Initial Setup", kHeader);
    interface.AddText(
        0, line++, "Welcome to Virtus. Let's set up your profile.", kSubheader);
    int index = 0;
    for (const auto& input : inputs) {
      int style = kNormal;
      if (index == button_index_y) {
        style |= kReverse;
      }
      interface.AddText(0, line++, input.first + ": " + input.second, style);
      index++;
    }
    int style = kNormal;
    if (button_index_y == index) {
      style |= kReverse;
    }
    interface.AddText(0, line++, "Finish", style | kBold);
    interface.Draw(Tab::kNone, -1, true);

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
      case 'q':
        throw std::runtime_error("User exited onboarding");
      case '\n':
        if (button_index_y < inputs.size()) {
          echo();
          char input[100];
          move(LINES - 1, 0);
          clrtoeol();
          mvprintw(LINES - 1, 0, "Enter %s: ", inputs.begin()->first.c_str());
          getnstr(input, 99);
          noecho();
          std::next(inputs.begin(), button_index_y)->second =
              std::string(input);
        } else {
          bool all_filled = true;
          for (const auto& input : inputs) {
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
        break;
    }
  }
}

void onboard::Introduce() {
  Interface interface;
  int line = 3;
  interface.AddText(0, line++, "Welcome to Virtus!", kHeader);
  interface.AddText(0, line++,
                    "Virtus is your all-in-one academic management tool.",
                    kSubheader);
  interface.AddText(0, line++, "With Virtus, you can:");
  interface.AddText(2, line++, "- Track assignments and due dates");
  interface.AddText(2, line++, "- Monitor your grades and performance");
  interface.AddText(2, line++, "- Manage your calendar");
  interface.AddText(0, line += 2, "Press any key to continue...", kItalic);
  interface.Draw(Tab::kNone, -1, true);
  auto ch = getch();
  switch (ch) {
    case 'q':
      throw std::runtime_error("User exited onboarding");
    default:
      break;
  }
  Interface tab_interface;
  int tab_line = 3;
  tab_interface.AddText(0, tab_line++, "Tabs", kHeader);
  tab_interface.AddText(
      0, tab_line++,
      "Above is the tab bar. Use it to navigate between different sections:",
      kSubheader);
  tab_interface.AddText(2, tab_line++,
                        "- Home: Overview of your academic status");
  tab_interface.AddText(2, tab_line++,
                        "- Assignments: View and manage your assignments");
  tab_interface.AddText(2, tab_line++,
                        "- Grades: Track your grades and performance");
  tab_interface.AddText(2, tab_line++, "- Calendar: Manage your schedule");
  tab_interface.AddText(2, tab_line++,
                        "- Settings: Customize your Virtus experience");
  tab_interface.AddText(0, tab_line += 2,
                        "Press any key to finish the introduction...", kItalic);
  tab_interface.Draw(Tab::kNone, -1, true);
  auto ch2 = getch();
  switch (ch2) {
    case 'q':
      throw std::runtime_error("User exited onboarding");
    default:
      break;
  }
}
