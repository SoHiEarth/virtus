#include "calendar.h"
#include "assignment.h"
#include "interface.h"
#include <map>
#include <format>

std::map<int, std::string> month_names = {
  {0, "January"}, {1, "February"}, {2, "March"}, {3, "April"},
  {4, "May"}, {5, "June"}, {6, "July"}, {7, "August"},
  {8, "September"}, {9, "October"}, {10, "November"}, {11, "December"}
};

Tab Calendar() {
  keypad(stdscr, TRUE);
  int button_index_x = 3, button_index_y = 0;
  time_t t = time(nullptr);
  tm* now = localtime(&t);

  int year = now->tm_year + 1900;
  int month = now->tm_mon; // 0–11

  tm first = {};
  first.tm_year = now->tm_year;
  first.tm_mon = month;
  first.tm_mday = 1;
  mktime(&first);

  int start_day = first.tm_wday;
  int days_in_month;
  if (month == 1) {
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    days_in_month = leap ? 29 : 28;
  } else if (month == 3 || month == 5 || month == 8 || month == 10) {
    days_in_month = 30;
  } else {
    days_in_month = 31;
  }

  

  while (true) {
    std::map<int, Assignment*> assignments_for_month;
    auto assignments = LoadAssignmentsFromDatabase();
    for (auto& assignment : assignments) {
      int a_year, a_month, a_day;
      sscanf(assignment.due_date.c_str(), "%d-%d-%d", &a_year, &a_month, &a_day);
      if (a_year == year && a_month - 1 == month) {
        assignments_for_month[a_day] = &assignment;
      }
    }
  
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, std::format("Calendar (Year: {}, Month: {})", year, month_names.at(month)), HEADER);
    interface.Draw(3, button_index_y == 0 ? button_index_x : -1);
    mvprintw(line++, interface_config::padding_left + 2, "Su       |Mo       |Tu       |We       |Th       |Fr       |Sa        ");
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 20; j++) {
        mvprintw(3*(i + 1) + 1, j + interface_config::padding_left + 2, "-");
      }
      printw("\n");
      for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 70; j++) {
          mvprintw(3 + i * 3 + 1, j + interface_config::padding_left + 2, "-");
        }
      }
      int row = 0;
      int col = start_day;
      for (int day = 1; day <= days_in_month; day++) {
        mvprintw(4 + row * 3 + 1, col * 10 + interface_config::padding_left + 2, "%2d", day);
        col++;
        if (col == 7) {
          col = 0;
          row++;
        }
      }  
    }
    line = 19;
    mvprintw(line++, interface_config::padding_left + 2, "Current Date: %04d-%02d-%02d\n", year, month + 1, now->tm_mday);
    int width = getmaxx(stdscr);
    for (int i = 0; i < width; ++i) {
      mvprintw(line, i, "-");
    }
    line++;
    move(line++, interface_config::padding_left);
    SetHeader("Assignment Info");
    int display_index = 0;
    for (int i = 1; i <= days_in_month; ++i) {
      if (assignments_for_month.find(i) != assignments_for_month.end()) {
        auto assignment = assignments_for_month.at(i);
        if (button_index_y - 1 == display_index)
          attron(A_BOLD | A_REVERSE);
        mvprintw(line + display_index, interface_config::padding_left + 2, "On %02d/%02d: %s (Class: %s)\n", 
          month + 1, i, assignment->name.c_str(), assignment->class_name.c_str());
        if (button_index_y - 1 == display_index)
          attroff(A_BOLD | A_REVERSE);
        display_index++;
      }
    }
    line++;
    refresh();
    auto ch = getch();
    if (ch == '\n') {
      if (button_index_y == 0) {
        if (button_index_x == 0) {
          return Tab::HOME;
        } else if (button_index_x == 1) {
          return Tab::ASSIGNMENTS;
        } else if (button_index_x == 2) {
          return Tab::GRADES;
        } else if (button_index_x == 4) {
          return Tab::SETTINGS;
        }
      } else {
        int display_index = 0;
        for (auto& [day, assignment] : assignments_for_month) {
          if (display_index == button_index_y - 1) {
            AssignmentMenu(assignment);
            SaveAssignmentsToDatabase(assignments);
            break;
          }
          display_index++;
        }
      }
    } else if (ch == KEY_LEFT) {
      if (button_index_x > 0) {
        --button_index_x;
      }
    } else if (ch == KEY_RIGHT) {
      if (button_index_x < 4) {
        ++button_index_x;
      }
    } else if (ch == KEY_UP) {
      if (button_index_y > 0) {
        --button_index_y;
      }
      button_index_x = 0;
    } else if (ch == KEY_DOWN) {
      if (button_index_y < assignments_for_month.size()) {
        ++button_index_y;
      }
      button_index_x = 0;
    } else if (ch == 'q' || ch == 'Q') {
      return Tab::NONE;
    }
  }
}
