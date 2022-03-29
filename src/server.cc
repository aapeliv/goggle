#include <iostream>

#include "glog/logging.h"
#include "httplib.h"

int main() {
  LOG(INFO) << "Starting Wikipedia Goggle.";

  httplib::Server srv;
  srv.Get("/", [](const httplib::Request &, httplib::Response &res) {
    res.set_content("{\"msg\": \"hello world\"}",
                    "application/json; charset=utf-8");
  });

  LOG(INFO) << "Serving on 8080.";

  srv.listen("0.0.0.0", 8080);
}
