#include "src/doc.h"

#include <iostream>
#include <memory>
#include <vector>

#include "gtest/gtest.h"

TEST(Document, Basic) {
  auto columbia = std::make_unique<Document>(1, "Columbia", "NY, NY",
                                             std::vector<std::string>{});
  auto brown = std::make_unique<Document>(2, "Brown", "Providence, RI",
                                          std::vector<std::string>{});
  auto princeton = std::make_unique<Document>(3, "Princeton", "Princeton, NJ",
                                              std::vector<std::string>{});
  auto harvard = std::make_unique<Document>(4, "Harvard", "Cambridge, MA",
                                            std::vector<std::string>{});
  auto dartmouth = std::make_unique<Document>(5, "Dartmouth", "Hanover, NH",
                                              std::vector<std::string>{});
  auto cornell = std::make_unique<Document>(6, "Cornell", "Cornell, NY",
                                            std::vector<std::string>{});
  auto penn = std::make_unique<Document>(7, "UPenn", "Pennsylvania, PA",
                                         std::vector<std::string>{});
  auto yale = std::make_unique<Document>(8, "Yale", "New Haven, CT",
                                         std::vector<std::string>{});

  std::cout << columbia->get_title() << "\n";
  std::cout << brown->get_title() << "\n";
}
