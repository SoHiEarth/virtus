#pragma once
#include <vector>
#include <pugixml.hpp>
#include <ncurses.h>
#include "interface.h"

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
void SaveAssignmentsToDatabase(const std::vector<Assignment>& assignments);
Assignment NewAssignment();
void EditAssignment(Assignment& assignment);
void AssignmentMenu(Assignment& assignment);
Tab Assignments();
