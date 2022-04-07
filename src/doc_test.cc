#include "src/doc.h"

#include <iostream>

#include "gtest/gtest.h"

TEST(Document, Basic) {
  Document *columbia = new Document(1, "Columbia", "NY, NY");
  Document *brown = new Document(2, "Brown", "Providence, RI");
  Document *princeton = new Document(3, "Princeton", "Princeton, NJ");
  Document *harvard = new Document(4, "Harvard", "Cambridge, MA");
  Document *dartmouth = new Document(5, "Dartmouth", "Hanover, NH");
  Document *cornell = new Document(6, "Cornell", "Cornell, NY");
  Document *penn = new Document(7, "UPenn", "Pennsylvania, PA");
  Document *yale = new Document(8, "Yale", "New Haven, CT");

  std::cout << columbia->get_title() << "\n";
  std::cout << brown->get_title() << "\n";
}
