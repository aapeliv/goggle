#include <iostream>
#include <string>

class Document {
 public:
  Document(std::string doc_title, uint32_t wiki_id, std::string txt) {
    title = doc_title;
    id = wiki_id;
    text = txt;
  }
  Document() {}
  std::string get_title() {
    return title;
  }

  std::string get_text() {
    return text;
  }

  uint32_t get_id() {
    return id;
  }

 private:
  std::string title;
  uint32_t id;
  std::string text;
};

int main() {
  Document *columbia = new Document("Columbia", 1, "NY, NY");
  Document *brown = new Document("Brown", 2, "Providence, RI");
  Document *princeton = new Document("Princeton", 3, "Princeton, NJ");
  Document *harvard = new Document("Harvard", 4, "Cambridge, MA");
  Document *dartmouth = new Document("Dartmouth", 5, "Hanover, NH");
  Document *cornell = new Document("Cornell", 6, "Cornell, NY");
  Document *penn = new Document("UPenn", 7, "Pennsylvania, PA");
  Document *yale = new Document("Yale", 8, "New Haven, CT");

  std::cout << columbia->get_title() << "\n";
  std::cout << brown->get_title() << "\n";
}
