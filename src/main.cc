#include "assignment.h"
#include "calendar.h"
#include "grades.h"
#include "home.h"
#include "interface.h"
#include "settings.h"
#include <filesystem>
#include <ncurses.h>
#include <pugixml.hpp>
#include "onboarding.h"
#include "class.h"

int main() {
  initscr();
  clear();
  if (!std::filesystem::exists("user_settings.xml")) {
    try {
    onboard::Setup();
    onboard::Introduce();
    } catch (const std::exception &e) {
      endwin();
      return 1;
    }
  } else {
    try {
      LoadUserSettings();
    } catch (const std::exception &e) {
      printw("Error loading user settings: %s\n", e.what());
      printw("Starting initial setup...\n");
      refresh();
      getch();
      try {
        onboard::Setup();
        onboard::Introduce();
      } catch (const std::exception &e) {
        endwin();
        return 1;
      }
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
    case Tab::CLASSES:
      current_tab = Classes();
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
