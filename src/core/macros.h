#pragma once
#include <chrono>
#include <iostream>
#include <string>
namespace cui {

using timePoint = std::chrono::steady_clock::time_point;

using text_color = int16_t;
enum : text_color {
  TEXT_COLOR_NORMAL = 0,
  TEXT_COLOR_ERROR = 1,
  TEXT_COLOR_WARNING = 2,
  TEXT_COLOR_NOTIFICATION = 3
};

#ifndef LOGS

#define LOG(text, color)                                                       \
  macros_functions::log(std::string("") + text, color, __FILE__, __LINE__)
#define ASSERT(condition, text, color)                                         \
  if (condition) {                                                             \
    macros_functions::log(std::string("") + text, color, __FILE__, __LINE__);  \
    throw std::runtime_error("Fatal error");                                   \
  }
#define GET_ELAPSED()                                                          \
  macros_functions::getElapsedTime<std::chrono::milliseconds>()

#define VK_CHECK(function, text)                                               \
  if (function != VK_SUCCESS) {                                                \
    macros_functions::log(std::string(text), TEXT_COLOR_ERROR, __FILE__,       \
                          __LINE__);                                           \
    throw std::runtime_error("Vulkan error");                                  \
  }

#define VIRTUAL_CALL_WARNING()                                                 \
  LOG("Warning this is a call from a virtual function, this should be "        \
      "overwritten by some derived class.",                                    \
      TEXT_COLOR_WARNING)

#else
#define LOG(text, color) ;
#define ASSERT(condition, text, color) ;
#define VK_CHECK(function) ;
#define GET_ELAPSED() ;
#define VIRTUAL_CALL_WARNING() ;

#endif
namespace macros_functions {

inline void log(const std::string data, text_color color, const char *file,
                const int line) {
  if (color == TEXT_COLOR_NORMAL)
    std::cout << "- ";
  else if (color == TEXT_COLOR_ERROR)
    std::cout << "\033[1;31m"
              << "-Error: ";
  else if (color == TEXT_COLOR_NOTIFICATION)
    std::cout << "\033[1;36m"
              << "-Notification: ";
  else if (color == TEXT_COLOR_WARNING)
    std::cout << "\033[1;33m"
              << "-Warning: ";
  else
    std::cout << "- ";

  std::cout << "In file: " << file << " In line; " << line
            << "\n\t Data:  " << data << "\033[0m"
            << "\n";
}

template <class chronoTimeMesure = std::chrono::nanoseconds>
inline long getElapsedTime() {
  std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
  static timePoint last{};
  long time = std::chrono::duration_cast<chronoTimeMesure>(now - last).count();
  last = now;
  return time;
}

} // namespace macros_functions

} // namespace cui
