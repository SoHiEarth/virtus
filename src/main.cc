#include <ncurses.h>

#include <filesystem>
#include <pugixml.hpp>

#include "assignment.h"
#include "calendar.h"
#include "class.h"
#include "grades.h"
#include "home.h"
#include "interface.h"
#include "onboarding.h"
#include "settings.h"

int main() {
  initscr();
  clear();
  if (!std::filesystem::exists("user_settings.xml")) {
    try {
      onboard::Setup();
      onboard::Introduce();
    } catch (const std::exception& e) {
      endwin();
      return 1;
    }
  } else {
    try {
      LoadUserSettings();
    } catch (const std::exception& e) {
      printw("Error loading user settings: %s\n", e.what());
      printw("Starting initial setup...\n");
      refresh();
      getch();
      try {
        onboard::Setup();
        onboard::Introduce();
      } catch (const std::exception& e) {
        endwin();
        return 1;
      }
    }
  }
  Tab current_tab = Tab::kHome;
  while (current_tab != Tab::kNone) {
    switch (current_tab) {
      case Tab::kHome:
        current_tab = Home();
        break;
      case Tab::kAssignments:
        current_tab = Assignments();
        break;
      case Tab::kClasses:
        current_tab = Classes();
        break;
      case Tab::kGrades:
        current_tab = Grades();
        break;
      case Tab::kCalendar:
        current_tab = Calendar();
        break;
      case Tab::kSettings:
        current_tab = Settings();
        break;
      default:
        current_tab = Tab::kNone;
        break;
    }
  }
  endwin();
  return 0;
}
