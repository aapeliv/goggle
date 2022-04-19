#include "src/doc.h"

#include <iostream>
#include <memory>

#include "gtest/gtest.h"

TEST(Document, Basic) {
  auto columbia = std::make_unique<Document>(1, "Columbia", "NY, NY");
  auto brown = std::make_unique<Document>(2, "Brown", "Providence, RI");
  auto princeton = std::make_unique<Document>(3, "Princeton", "Princeton, NJ");
  auto harvard = std::make_unique<Document>(4, "Harvard", "Cambridge, MA");
  auto dartmouth = std::make_unique<Document>(5, "Dartmouth", "Hanover, NH");
  auto cornell = std::make_unique<Document>(6, "Cornell", "Cornell, NY");
  auto penn = std::make_unique<Document>(7, "UPenn", "Pennsylvania, PA");
  auto yale = std::make_unique<Document>(8, "Yale", "New Haven, CT");

  std::cout << columbia->get_title() << "\n";
  std::cout << brown->get_title() << "\n";
}
