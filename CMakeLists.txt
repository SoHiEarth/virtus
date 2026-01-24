cmake_minimum_required(VERSION 3.10)
project(virtus)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(Curses REQUIRED)
if(Curses_FOUND AND NOT TARGET Curses::Curses)
  add_library(Curses::Curses INTERFACE IMPORTED)
  set_target_properties(
    Curses::Curses
    PROPERTIES
      INTERFACE_LINK_LIBRARIES "${CURSES_LIBRARIES}"
      INTERFACE_INCLUDE_DIRECTORIES "${CURSES_INCLUDE_DIRS}"
  )
endif()

find_package(pugixml REQUIRED)
add_executable(virtus)
target_sources(virtus PRIVATE
  src/main.cc
  src/class.cc
  src/assignment.cc
  src/calculate.cc
  src/interface.cc
  src/home.cc
  src/settings.cc
  src/grades.cc
  src/onboarding.cc
  src/calendar.cc
)
target_include_directories(virtus PRIVATE include)
target_link_libraries(virtus PRIVATE Curses::Curses pugixml::pugixml)
