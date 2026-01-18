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

void DrawTabBar(std::vector<std::string> tabs, int active_index, int focused_index, bool show_app_title, bool disabled_tab_bar) {
  int width = getmaxx(stdscr);
  if (disabled_tab_bar) {
    attron(A_DIM);
  }
  if (!interface_config::simple_tab_bar) {
    for (int i = 0; i < width; ++i) {
      mvprintw(0, i, "-");
    }
  }
  move(interface_config::simple_tab_bar ? 0 : 1, 1);
  if (show_app_title) {
    attron(A_ITALIC | A_DIM);
    printw("Virtus ");
    attroff(A_ITALIC | A_DIM);
  }
  if (disabled_tab_bar) {
    attron(A_DIM);
  }
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
  if (!interface_config::simple_tab_bar) {
    for (int i = 0; i < width; ++i) {
      mvprintw(2, i, "-");
    }
  }
  if (disabled_tab_bar) {
    attroff(A_DIM);
  }
}

