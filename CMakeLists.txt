cmake_minimum_required(VERSION 3.10)
project(virtus)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(Curses REQUIRED)
find_package(pugixml REQUIRED)
add_executable(virtus)
target_sources(virtus PRIVATE
  src/main.cc
  src/assignment.cc
  src/interface.cc
  src/home.cc
  src/settings.cc
  src/grades.cc
  src/calendar.cc
)
target_include_directories(virtus PRIVATE include)
target_link_libraries(virtus PRIVATE Curses pugixml::pugixml)
