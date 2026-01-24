#pragma once
#include <ncurses.h>
#include <string>
#include <vector>
enum class Tab : int {
  HOME = 0,
  ASSIGNMENTS,
  CLASSES,
  GRADES,
  CALENDAR,
  SETTINGS,
  NONE
};

struct Button {
  const char *label;
  bool focused;
  void Draw(bool notab = false) {
    if (focused) {
      attron(A_REVERSE);
    }
    printw("[ %s ]", label);
    if (focused) {
      attroff(A_REVERSE);
    }
    if (!notab)
      printw("\t");
  }
};

enum Style : int {
  NORMAL = 0,
  BOLD = 1 << 0,
  ITALIC = 1 << 1,
  DIM = 1 << 2,
  REVERSE = 1 << 3,
  HEADER = 1 << 4,
  SUBHEADER = 1 << 5,
};

struct TextObject {
  int x = 0;
  int y = 0;
  std::string content;
  int style;
};

void SetHeader(const char *title);
void SetSubHeader(const char *subtitle);
void DrawTabBar(std::vector<std::string> tabs, Tab active_tab,
                int focused_index = 0, bool show_app_title = true,
                bool disabled_tab_bar = false);

namespace interface_config {
inline int padding_left = 2;
inline bool show_app_title = true;
inline bool simple_tab_bar = false;
inline bool disable_fancy_text = false;
} // namespace interface_config

struct Interface {
  std::vector<TextObject> texts;

  void AddText(int x, int y, std::string_view content, int style = NORMAL) {
    texts.push_back({x, y, std::string(content), style});
  }

  void Draw(Tab active_tab, int focused_tab = -1,
            bool disabled_tab_bar = false) {
    clear();
    DrawTabBar(
        {"Home", "Assignments", "Classes", "Grades", "Calendar", "Settings"},
        active_tab, focused_tab, interface_config::show_app_title,
        disabled_tab_bar);
    int indent_level = 0;
    for (const auto &text : texts) {
      move(text.y, text.x);
      for (int i = 0; i < interface_config::padding_left; ++i)
        printw(" ");
      if (text.style & HEADER) {
        indent_level = 0;
      } else if (text.style & SUBHEADER) {
        indent_level = 1;
      }
      for (int i = 0; i < indent_level; ++i) {
        printw("  ");
      }
      if (text.style & BOLD || text.style & HEADER || text.style & SUBHEADER) {
        attron(A_BOLD);
      }
      if (text.style & ITALIC) {
        attron(A_ITALIC);
      }
      if (text.style & DIM || text.style & SUBHEADER) {
        attron(A_DIM);
      }
      if (text.style & REVERSE) {
        attron(A_REVERSE);
      }
      if (interface_config::disable_fancy_text) {
        attroff(A_BOLD | A_ITALIC | A_DIM);
      }
      printw("%s", text.content.c_str());
      attroff(A_BOLD | A_ITALIC | A_DIM | A_REVERSE);
      if (text.style & HEADER) {
        indent_level = 1;
      } else if (text.style & SUBHEADER) {
        indent_level = 2;
      }
    }
    move(LINES - 2, 2);
    printw("Use arrow keys to navigate tabs, Enter to select, Q to quit.");
    refresh();
  }
};
