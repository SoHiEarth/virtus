#include "settings.h"

#include <cstring>
#include <filesystem>
#include <functional>
#include <pugixml.hpp>

#include "calculate.h"
#include "interface.h"

constexpr int kSettingNameWidth = 30;
#define SETTING_STRING(display_name, var_name) \
  {SETTING, display_name,                      \
   []() {                                      \
     echo();                                   \
     char input[100];                          \
     mvprintw(LINES - 1, 0, "Enter Value:");   \
     getnstr(input, 99);                       \
     (var_name) = std::string(input);          \
   },                                          \
   []() { return var_name; }}
#define SETTING_BOOL(display_name, var_name)                  \
  {SETTING, display_name, []() { (var_name) = !(var_name); }, \
   []() { return (var_name) ? "On" : "Off"; }}
#define SETTING_INT(display_name, var_name)                  \
  {SETTING, display_name,                                    \
   []() {                                                    \
     echo();                                                 \
     char input[100] = {0};                                  \
     move(LINES - 1, 0);                                     \
     clrtoeol();                                             \
     mvprintw(LINES - 1, 0, "Enter Value: ");                \
     getnstr(input, 99);                                     \
     noecho();                                               \
     try {                                                   \
       if (strlen(input) > 0) (var_name) = std::stoi(input); \
     } catch (const std::exception&) {                       \
     }                                                       \
   },                                                        \
   []() { return std::to_string(var_name); }}

namespace settings {
bool error_logging = false;
}  // namespace settings

namespace user_settings {
std::string school_name = "Default School";
std::string student_first_name = "John";
std::string student_last_name = "Doe";
}  // namespace user_settings

namespace {
enum SettingTypeMarker { kSeperator, SETTING };
struct Setting {
  SettingTypeMarker type = SETTING;
  std::string name;
  std::function<void()> toggle = []() {};
  std::function<std::string()> get_status = []() { return ""; };
};
}  // namespace

void LoadUserSettings() {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("user_settings.xml");
  if (!result) {
    throw std::runtime_error("Failed to load user_settings.xml");
  }
  pugi::xml_node settings = doc.child("settings");
  user_settings::school_name = settings.child("school_name").text().as_string();
  user_settings::student_first_name =
      settings.child("student_first_name").text().as_string();
  user_settings::student_last_name =
      settings.child("student_last_name").text().as_string();
  user_settings::gpa_calculation_method =
      static_cast<Method>(settings.child("gpa_calculation_method")
                              .text()
                              .as_int(static_cast<int>(kAverage)));
}

void SaveUserSettings() {
  pugi::xml_document doc;
  pugi::xml_node settings = doc.append_child("settings");
  settings.append_child("school_name").text() =
      user_settings::school_name.c_str();
  settings.append_child("student_first_name").text() =
      user_settings::student_first_name.c_str();
  settings.append_child("student_last_name").text() =
      user_settings::student_last_name.c_str();
  settings.append_child("gpa_calculation_method").text() =
      static_cast<int>(user_settings::gpa_calculation_method);
  doc.save_file("user_settings.xml");
}

Tab Settings() {
  keypad(stdscr, TRUE);
  int button_index_x = 5;
  int button_index_y = 0;
  std::vector<Setting> settings_list = {
      {.type = kSeperator, .name = "Assignment Settings"},
      SETTING_BOOL("Error Logging", settings::error_logging),
      {.type = SETTING,
       .name = "Reset User Settings",
       .toggle = []() { std::filesystem::remove("user_settings.xml"); },
       .get_status = []() { return ""; }},
      {.type = kSeperator, .name = "Profile Settings"},
      SETTING_STRING("School Name", user_settings::school_name),
      SETTING_STRING("First Name", user_settings::student_first_name),
      SETTING_STRING("Last Name", user_settings::student_last_name),
      {.type = kSeperator, .name = "Interface Settings"},
      SETTING_BOOL("Show App Title", interface_config::show_app_title),
      SETTING_BOOL("Simple Tab Bar", interface_config::simple_tab_bar),
      SETTING_BOOL("Disable Fancy Text", interface_config::disable_fancy_text),
      SETTING_INT("Padding Left", interface_config::padding_left),
      {.type = kSeperator, .name = "Grade Settings"},
      {.type = SETTING,
       .name = "GPA Calculation Method",
       .toggle =
           []() {
             if (user_settings::gpa_calculation_method == kAverage) {
               user_settings::gpa_calculation_method =
                   kInternationalBaccalaureate;
             } else {
               user_settings::gpa_calculation_method = kAverage;
             }
           },
       .get_status =
           []() {
             return (user_settings::gpa_calculation_method == kAverage)
                        ? "Average"
                        : "International Baccalaureate";
           }}};
  std::vector<size_t> selectable_indices;
  for (size_t i = 0; i < settings_list.size(); ++i) {
    if (settings_list[i].type == SETTING) {
      selectable_indices.push_back(i);
    }
  }

  while (true) {
    Interface interface;
    int line = interface_config::simple_tab_bar ? 1 : 3;
    interface.AddText(0, line++, "General", kHeader);
    int setting_index = 0;
    for (const auto& setting : settings_list) {
      if (setting.type == kSeperator) {
        interface.AddText(0, line++, setting.name, kSubheader);
      } else {
        setting_index++;
        int style = kNormal;
        if (button_index_y == setting_index && button_index_x == 0) {
          style |= kReverse;
        }
        interface.AddText(0, line, setting.name, style | kBold);
        interface.AddText(kSettingNameWidth, line++, setting.get_status(),
                          style);
      }
    }
    interface.Draw(Tab::kSettings, button_index_y == 0 ? button_index_x : -1);

    auto ch = getch();
    switch (ch) {
      case KEY_UP:
        if (button_index_y > 0) {
          --button_index_y;
        }
        button_index_x = 0;
        break;
      case KEY_DOWN:
        if (button_index_y < settings_list.size() - 4) {
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
        if (button_index_x < 4) {
          ++button_index_x;
        }
        break;
      case '\n':
        if (button_index_y == 0) {
          if (button_index_x == static_cast<int>(Tab::kHome)) {
            return Tab::kHome;
          }
          if (button_index_x == static_cast<int>(Tab::kAssignments)) {
            return Tab::kAssignments;
          }
          if (button_index_x == static_cast<int>(Tab::kClasses)) {
            return Tab::kClasses;
          }
          if (button_index_x == static_cast<int>(Tab::kGrades)) {
            return Tab::kGrades;
          }
          if (button_index_x == static_cast<int>(Tab::kCalendar)) {
            return Tab::kCalendar;
          }
        } else {
          settings_list.at(selectable_indices.at(button_index_y - 1)).toggle();
        }
        break;
      case 'q':
      case 'Q':
        return Tab::kNone;
      default:
        break;
    }
  }
  SaveUserSettings();
  return Tab::kNone;
}
