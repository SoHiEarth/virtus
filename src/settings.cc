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
  std::vector<Setting> profile_settings = {
      {"School Name", []() {}, []() { return user_settings::school_name; }},
      {"First Name: ", []() {}, []() { return user_settings::student_first_name; }},
      {"Last Name: ", []() {}, []() { return user_settings::student_last_name; }},
  };
  std::vector<Setting> interface_settings = {
    {"Show App Title", []() { interface_config::show_app_title = !interface_config::show_app_title; }, []() { return interface_config::show_app_title ? "On" : "Off"; }},
    {"Simple Tab Bar", []() { interface_config::simple_tab_bar = !interface_config::simple_tab_bar; }, []() { return interface_config::simple_tab_bar ? "On" : "Off"; }},
    {"Disable Fancy Text", []() { interface_config::disable_fancy_text = !interface_config::disable_fancy_text; }, []() { return interface_config::disable_fancy_text ? "On" : "Off"; }},
    {"Padding Left", []() { interface_config::padding_left = (interface_config::padding_left == 0) ? 2 : 0; }, []() { return std::to_string(interface_config::padding_left); }},
  };

  while (true) {
    Interface interface;
    interface.AddText(0, 3, "General", HEADER);

    interface.AddText(0, 4, "Assignment Management", SUBHEADER);
    for (size_t i = 0; i < settings_list.size(); ++i) {
      int style = NORMAL;
      if (i == button_index_y - 1 && button_index_x == 0) {
        style |= REVERSE;
      }
      interface.AddText(0, i + 5, settings_list[i].name, style | BOLD);
      interface.AddText(SETTING_NAME_WIDTH, i + 5, settings_list[i].get_status(), style);
    }

    interface.AddText(0, 5 + settings_list.size(), "User Profile", SUBHEADER);
    for (size_t i = 0; i < profile_settings.size(); ++i) {
      int style = NORMAL;
      if (i + settings_list.size() + 1 == button_index_y && button_index_x == 0) {
        style |= REVERSE;
      }
      interface.AddText(0, i + 6 + settings_list.size(), profile_settings[i].name, style | BOLD);
      interface.AddText(SETTING_NAME_WIDTH, i + 6 + settings_list.size(), profile_settings[i].get_status(), style);
    }

    interface.AddText(0, 7 + settings_list.size() + profile_settings.size(), "Interface", SUBHEADER);
    for (size_t i = 0; i < interface_settings.size(); ++i) {
      int style = NORMAL;
      if (i + settings_list.size() + profile_settings.size() + 1 == button_index_y && button_index_x == 0) {
        style |= REVERSE;
      }
      interface.AddText(0, i + 8 + settings_list.size() + profile_settings.size(), interface_settings[i].name, style | BOLD);
      interface.AddText(SETTING_NAME_WIDTH, i + 8 + settings_list.size() + profile_settings.size(), interface_settings[i].get_status(), style);
    }
    interface.Draw(true, 3, button_index_y == 0 ? button_index_x : -1);

    auto ch = getch();
    switch (ch) {
      case KEY_UP:
        if (button_index_y > 0) {
          --button_index_y;
        }
        button_index_x = 0;
        break;
      case KEY_DOWN:
        if (button_index_y < settings_list.size() + profile_settings.size() + interface_settings.size()) {
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
          else if (button_index_y - 1 < settings_list.size() + profile_settings.size()) {
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
          } else {
            size_t interface_index = button_index_y - 1 - settings_list.size() - profile_settings.size();
            interface_settings[interface_index].toggle();
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
