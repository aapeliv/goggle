#include "src/doc.h"

#include <iostream>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

TEST(Document, Basic) {
  auto columbia = std::make_unique<Document>(1, 77, "Columbia", "NY, NY",
                                             std::vector<std::string>{});
  auto brown = std::make_unique<Document>(2, 23, "Brown", "Providence, RI",
                                          std::vector<std::string>{});
  auto princeton = std::make_unique<Document>(
      3, 12, "Princeton", "Princeton, NJ", std::vector<std::string>{});
  auto harvard = std::make_unique<Document>(4, 55, "Harvard", "Cambridge, MA",
                                            std::vector<std::string>{});
  auto dartmouth = std::make_unique<Document>(5, 11, "Dartmouth", "Hanover, NH",
                                              std::vector<std::string>{});
  auto cornell = std::make_unique<Document>(6, 45, "Cornell", "Cornell, NY",
                                            std::vector<std::string>{});
  auto penn = std::make_unique<Document>(7, 74, "UPenn", "Pennsylvania, PA",
                                         std::vector<std::string>{});
  auto yale = std::make_unique<Document>(8, 94, "Yale", "New Haven, CT",
                                         std::vector<std::string>{});

  std::cout << columbia->get_title() << "\n";
  std::cout << brown->get_title() << "\n";
}
