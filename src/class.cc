#include "class.h"
#include "interface.h"

Tab Classes() {
  keypad(stdscr, TRUE);
  int button_index_x = 2, button_index_y = 0;
  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, "Your Classes", HEADER);
    interface.AddText(0, line++,
                      "This feature is under development. Stay tuned for updates!",
                      NORMAL | ITALIC);

    interface.Draw(Tab::CLASSES, button_index_y == 0 ? button_index_x : -1);
    auto ch = getch();
    switch (ch) {
    case KEY_UP:
      if (button_index_y > 0) {
        --button_index_y;
      }
      button_index_x = 0;
      break;
    case KEY_LEFT:
      if (button_index_x > 0) {
        --button_index_x;
      }
      break;
    case KEY_RIGHT:
      if (button_index_x < static_cast<int>(Tab::SETTINGS)) {
        ++button_index_x;
      }
      break;
    case '\n':
      if (button_index_x == static_cast<int>(Tab::HOME)) {
        return Tab::HOME;
      } else if (button_index_x == static_cast<int>(Tab::ASSIGNMENTS)) {
        return Tab::ASSIGNMENTS;
      } else if (button_index_x == static_cast<int>(Tab::GRADES)) {
        return Tab::GRADES;
      } else if (button_index_x == static_cast<int>(Tab::CALENDAR)) {
        return Tab::CALENDAR;
      } else if (button_index_x == static_cast<int>(Tab::SETTINGS)) {
        return Tab::SETTINGS;
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
