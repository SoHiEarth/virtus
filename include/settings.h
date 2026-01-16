#pragma once
#include "interface.h"

namespace settings {
  extern bool error_logging;
}

namespace user_settings {
  extern std::string school_name;
  extern std::string student_first_name;
  extern std::string student_last_name;
}

Tab Settings();
void LoadUserSettings();
void SaveUserSettings();
