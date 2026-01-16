#include "settings.h"
#include "interface.h"
#include <functional>
#include <filesystem>
#include <pugixml.hpp>

constexpr int SETTING_NAME_WIDTH = 30;

namespace settings {
  bool error_logging = false;
}  // namespace settings

namespace user_settings {
  std::string school_name = "Default School";
  std::string student_first_name = "John";
  std::string student_last_name = "Doe";
} // namespace user_settings

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

struct Setting {
  std::string name;
  std::function<void()> toggle;
  std::function<std::string()> get_status;
};

Tab Settings() {
  keypad(stdscr, TRUE);
  int button_index_x = 3, button_index_y = 0;
  std::vector<Setting> settings_list = {
    {"Error Logging", []() { settings::error_logging = !settings::error_logging; }, []() { return settings::error_logging ? "On" : "Off"; }},
    {"Reset User Settings", []() { std::filesystem::remove("user_settings.xml"); }, []() { return ""; }},
  };
  while (true) {
    clear();
    DrawTabBar({"Home", "Assignments", "Grades", "Settings"}, 3, button_index_y == 0 ? button_index_x : -1);
    move(3, 2);
    SetHeader("General");
    move(4, 4);
    SetSubHeader("Assignment Management");
    for (size_t i = 0; i < settings_list.size(); ++i) {
      if (i == button_index_y - 1 && button_index_x == 0) {
        attron(A_REVERSE);
      }
      mvprintw(i + 5, 6, "%s", settings_list[i].name.c_str());
      mvprintw(i + 5, 6 + SETTING_NAME_WIDTH, "%s\n", settings_list[i].get_status().c_str());
      if (i == button_index_y - 1 && button_index_x == 0) {
        attroff(A_REVERSE);
      }
    }

    move(5 + settings_list.size(), 4);
    SetSubHeader("User Profile");
    std::vector<Setting> profile_settings = {
      {"School Name", []() {}, []() { return user_settings::school_name; }},
      {"First Name: ", []() {}, []() { return user_settings::student_first_name; }},
      {"Last Name: ", []() {}, []() { return user_settings::student_last_name; }},
    };
    for (size_t i = 0; i < profile_settings.size(); ++i)
    {
      if (i + settings_list.size() + 1 == button_index_y && button_index_x == 0) {
        attron(A_REVERSE);
      }
      mvprintw(i + 6 + settings_list.size(), 6, "%s", profile_settings[i].name.c_str());
      mvprintw(i + 6 + settings_list.size(), 6 + SETTING_NAME_WIDTH, "%s\n", profile_settings[i].get_status().c_str());
      if (i + settings_list.size() + 1 == button_index_y && button_index_x == 0) {
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
        if (button_index_y < settings_list.size() + profile_settings.size()) {
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
          } else if (button_index_x == 1) {
            return Tab::ASSIGNMENTS;
          } else if (button_index_x == 2) {
            return Tab::GRADES;
          }
        } else {
          if (button_index_y - 1 < settings_list.size())
            settings_list[button_index_y - 1].toggle();
          else {
            size_t profile_index = button_index_y - 1 - settings_list.size();
            echo();
            char input[100];
            if (profile_index == 0) {
              mvprintw(button_index_y + 5, 0, "\t\tSchool Name: ");
              getnstr(input, 99);
              user_settings::school_name = std::string(input);
            } else if (profile_index == 1) {
              mvprintw(button_index_y + 5, 0, "\t\tFirst Name: ");
              getnstr(input, 99);
              user_settings::student_first_name = std::string(input);
            } else if (profile_index == 2) {
              mvprintw(button_index_y + 5, 0, "\t\tLast Name: ");
              getnstr(input, 99);
              user_settings::student_last_name = std::string(input);
            }
          }
        }
        break;
      case 'q':
      case 'Q':
        return Tab::NONE;
      default:
        break;
    }
  }
  SaveUserSettings();
  return Tab::NONE;
}
