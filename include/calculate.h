#pragma once
#include <vector>

enum Method { AVERAGE, INTERNATIONAL_BACCALAUREATE };
namespace user_settings {
extern Method gpa_calculation_method;
}
struct Assignment;
double CalculateGPA(const std::vector<Assignment> &assignments,
                    std::string class_name = "");
