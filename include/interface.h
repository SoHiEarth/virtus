#pragma once
#include <ncurses.h>
#include <vector>
#include <string>
enum class Tab {
    HOME,
    ASSIGNMENTS,
    GRADES,
    SETTINGS,
    NONE
};

struct Button {
  const char* label;
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

void SetHeader(const char* title);
void SetSubHeader(const char* subtitle);
void DrawTabBar(std::vector<std::string> tabs, int active_index, int focused_index = 0);
