#ifndef CALCULATE_H
#define CALCULATE_H

#include <string>
#include <vector>

enum Method { kAverage, kInternationalBaccalaureate };
namespace user_settings {
extern Method gpa_calculation_method;
}
struct Assignment;
double CalculateGPA(const std::vector<Assignment>& assignments,
                    const std::string& class_name = "");
double CalculateGrade(const std::vector<Assignment>& assignments,
                      const std::string& class_name = "");

#endif  // CALCULATE_H
