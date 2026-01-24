#include "calculate.h"
#include "assignment.h"

namespace user_settings {
Method gpa_calculation_method = AVERAGE;
}  // namespace user_settings

double CalculateGPA(const std::vector<Assignment> &assignments,
                 std::string class_name) {
  switch (user_settings::gpa_calculation_method) {
    case AVERAGE: {
      if (assignments.empty()) return 0;
      double total = 0;
      for (const auto &assignment : assignments) {
        if (!class_name.empty() && assignment.class_name != class_name) {
          continue;
        }
        total += assignment.score;
      }
      return std::clamp(total / assignments.size() / 20, 0.0, 5.0);
    }
    case INTERNATIONAL_BACCALAUREATE: {
      std::vector<double> ib_scores;
      for (const auto &assignment : assignments) {
        if (!class_name.empty() && assignment.class_name != class_name) {
          continue;
        }
        ib_scores.push_back((assignment.score / 100) * 7);
      }
      double total = 0;
      for (const auto &score : ib_scores) {
        total += score;
      }
      return std::clamp(total / ib_scores.size(), 0.0, 7.0);
    }
  }
  return 0;
}

double CalculateGrade(const std::vector<Assignment> &assignments,
                       std::string class_name) {
  if (assignments.empty()) return 0;
  double total = 0;
  int count = 0;
  for (const auto &assignment : assignments) {
    if (!class_name.empty() && assignment.class_name != class_name) {
      continue;
    }
    total += assignment.score;
    count++;
  }
  return count == 0 ? 0 : total / count;
}
