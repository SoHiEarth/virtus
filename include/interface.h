#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>

#include <string>
#include <vector>
enum class Tab : int {
  kHome = 0,
  kAssignments,
  kClasses,
  kGrades,
  kCalendar,
  kSettings,
  kNone
};

struct Button {
  const char* label;
  bool focused;
  void Draw(bool notab = false) const {
    if (focused) {
      attron(A_REVERSE);
    }
    printw("[ %s ]", label);
    if (focused) {
      attroff(A_REVERSE);
    }
    if (!notab) printw("\t");
  }
};

enum Style : int {
  kNormal = 0,
  kBold = 1 << 0,
  kItalic = 1 << 1,
  kDim = 1 << 2,
  kReverse = 1 << 3,
  kHeader = 1 << 4,
  kSubheader = 1 << 5,
};

struct TextObject {
  int x = 0;
  int y = 0;
  std::string content;
  int style;
};

void SetHeader(const char* title);
void SetSubHeader(const char* subtitle);
void DrawTabBar(std::vector<std::string> tabs, Tab active_tab,
                int focused_index = 0, bool show_app_title = true,
                bool disabled_tab_bar = false);

namespace interface_config {
inline int padding_left = 2;
inline bool show_app_title = true;
inline bool simple_tab_bar = false;
inline bool disable_fancy_text = false;
}  // namespace interface_config

struct Interface {
  std::vector<TextObject> texts;

  void AddText(int x, int y, std::string_view content, int style = kNormal) {
    texts.push_back(
        {.x = x, .y = y, .content = std::string(content), .style = style});
  }

  void Draw(Tab active_tab, int focused_tab = -1,
            bool disabled_tab_bar = false) {
    clear();
    DrawTabBar(
        {"Home", "Assignments", "Classes", "Grades", "Calendar", "Settings"},
        active_tab, focused_tab, interface_config::show_app_title,
        disabled_tab_bar);
    int indent_level = 0;
    for (const auto& text : texts) {
      move(text.y, text.x);
      for (int i = 0; i < interface_config::padding_left; ++i) printw(" ");
      if (text.style & kHeader) {
        indent_level = 0;
      } else if (text.style & kSubheader) {
        indent_level = 1;
      }
      for (int i = 0; i < indent_level; ++i) {
        printw("  ");
      }
      if (text.style & kBold || text.style & kHeader ||
          text.style & kSubheader) {
        attron(A_BOLD);
      }
      if (text.style & kItalic) {
        attron(A_ITALIC);
      }
      if (text.style & kDim || text.style & kSubheader) {
        attron(A_DIM);
      }
      if (text.style & kReverse) {
        attron(A_REVERSE);
      }
      if (interface_config::disable_fancy_text) {
        attroff(A_BOLD | A_ITALIC | A_DIM);
      }
      printw("%s", text.content.c_str());
      attroff(A_BOLD | A_ITALIC | A_DIM | A_REVERSE);
      if (text.style & kHeader) {
        indent_level = 1;
      } else if (text.style & kSubheader) {
        indent_level = 2;
      }
    }
    move(LINES - 2, 2);
    printw("Use arrow keys to navigate tabs, Enter to select, Q to quit.");
    refresh();
  }
};

#endif  // INTERFACE_H
