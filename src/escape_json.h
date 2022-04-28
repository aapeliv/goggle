// lifted from https://stackoverflow.com/a/33799784, under cc by-sa
#pragma once

#include <iomanip>
#include <sstream>

std::string escape_json(const std::string &s) {
  std::ostringstream o;
  for (auto c = s.cbegin(); c != s.cend(); c++) {
    if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f')) {
      o << "\\u" << std::hex << std::setw(4) << std::setfill('0')
        << static_cast<int>(*c);
    } else {
      o << *c;
    }
  }
  return o.str();
}
