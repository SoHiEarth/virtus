#pragma once
#include "interface.h"
#include <ncurses.h>
#include <pugixml.hpp>
#include <vector>

struct Assignment {
  std::string name = "";
  std::string class_name = "";
  std::string description = "";
  std::string due_date = "";
  bool completed = false;
  float score = 0;
  float max_score = 100;
};

std::vector<Assignment> LoadAssignmentsFromDatabase();
void SaveAssignmentsToDatabase(const std::vector<Assignment> &assignments);
Assignment NewAssignment();
void EditAssignment(Assignment &assignment);
void AssignmentMenu(Assignment &assignment);
void AssignmentMenu(Assignment *assignment);
Tab Assignments();
