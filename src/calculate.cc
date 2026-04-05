#include "calculate.h"

#include <algorithm>

#include "assignment.h"

namespace user_settings {
Method gpa_calculation_method = kAverage;
}  // namespace user_settings

double CalculateGPA(const std::vector<Assignment>& assignments,
                    const std::string& class_name) {
  switch (user_settings::gpa_calculation_method) {
    case kAverage: {
      if (assignments.empty()) return 0;
      double total = 0;
      for (const auto& assignment : assignments) {
        if (!class_name.empty() && assignment.class_name != class_name) {
          continue;
        }
        total += assignment.score;
      }
      return std::clamp(total / assignments.size() / 20, 0.0, 5.0);
    }
    case kInternationalBaccalaureate: {
      std::vector<double> ib_scores;
      for (const auto& assignment : assignments) {
        if (!class_name.empty() && assignment.class_name != class_name) {
          continue;
        }
        ib_scores.push_back((assignment.score / 100) * 7);
      }
      double total = 0;
      for (const auto& score : ib_scores) {
        total += score;
      }
      return std::clamp(total / ib_scores.size(), 0.0, 7.0);
    }
  }
  return 0;
}

double CalculateGrade(const std::vector<Assignment>& assignments,
                      const std::string& class_name) {
  if (assignments.empty()) return 0;
  double total = 0;
  int count = 0;
  for (const auto& assignment : assignments) {
    if (!class_name.empty() && assignment.class_name != class_name) {
      continue;
    }
    total += assignment.score;
    count++;
  }
  return count == 0 ? 0 : total / count;
}
