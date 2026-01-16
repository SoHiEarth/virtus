#include "interface.h"

void SetHeader(const char* title) {
  attron(A_BOLD);
  printw("%s\n", title);
  attroff(A_BOLD);
}

void SetSubHeader(const char* subtitle) {
  attron(A_BOLD | A_DIM);
  printw("%s\n", subtitle);
  attroff(A_BOLD | A_DIM);
}

void DrawTabBar(std::vector<std::string> tabs, int active_index, int focused_index) {
  int width = getmaxx(stdscr);
  for (int i = 0; i < width; ++i) {
    mvprintw(0, i, "-");
  }
  attron(A_ITALIC | A_DIM);
  mvprintw(1, 1, "Virtus ");
  attroff(A_ITALIC | A_DIM);
  for (size_t i = 0; i < tabs.size(); ++i) {
    printw("|");
    if (i == active_index) {
      attron(A_BOLD);
    }
    if (i == focused_index) {
      attron(A_REVERSE);
    }
    printw(" %s ", tabs[i].c_str());
    if (i == active_index) {
      attroff(A_BOLD);
    }
    if (i == focused_index) {
      attroff(A_REVERSE);
    }
  }
  printw("|");
  printw("\n");
  for (int i = 0; i < width; ++i) {
    mvprintw(2, i, "-");
  }
}

