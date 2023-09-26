#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "./b_plus.hpp"
#include "./httplib.h"

using namespace std;
using namespace httplib;

BPTree* tree;

void indexPage(const Request& req, Response& res) {
  printf("httplib server recv a req: %s\n ", req.path.c_str());
  ifstream fin("index.html");

  if (!fin) {
    cout << "[Error] template loading fault" << endl;

    string page;
    string str;
    while (getline(fin, str)) {
      page = page + str;
    }
    res.set_content(page, "text/html");
    res.status = 200;
  }
}

void getGraph(const Request& req, Response& res) {
  auto link = (req.has_param("link") && req.get_param_value("link") == "true");
  auto data = (req.has_param("data") && req.get_param_value("data") == "true");
  res.set_content(tree->graphviz(link, data), "text/plain");
  res.status = 200;
}

void insert(const Request& req, Response& res) {
  if (req.has_param("key") && req.has_param("value")) {
    auto key = req.get_param_value("key");
    auto value = req.get_param_value("value");
    auto link =
        (req.has_param("link") && req.get_param_value("link") == "true");
    auto data =
        (req.has_param("data") && req.get_param_value("data") == "true");
    tree->insert(stoi(key), value);
    cout << "[Insert] insert data \"" << data << "\" with key " << key << endl;
    res.set_content(tree->graphviz(link, data), "text/plain");
    res.status = 200;
  }
}

void _delete(const Request& req, Response& res) {
  if (req.has_param("key")) {
    auto key = req.get_param_value("key");
    auto link =
        (req.has_param("link") && req.get_param_value("link") == "true");
    auto data =
        (req.has_param("data") && req.get_param_value("data") == "true");
    tree->_delete(stoi(key));
    cout << "[Delete] delete key " << key << endl;
    res.set_content(tree->graphviz(link, data), "text/plain");
    res.status = 200;
  }
}

void query(const Request& req, Response& res) {
  if (req.has_param("key")) {
    auto key = req.get_param_value("key");
    Data* query_res = tree->query(stoi(key));
    if (query_res == nullptr) {
      res.set_content("{\"found\":false}", "text/plain");
      cout << "[Query] not found any data with key " << key << endl;
    } else {
      res.set_content(
          "{\"found\":true,\"values\":\"" + query_res->dumpValue() + "\"}",
          "text/plain");
      cout << "[Query] found data \"" << query_res->dumpValue()
           << "\" with key " << key << endl;
    }
    res.status = 200;
  }
}

void init(const Request& req, Response& res) {
  if (req.has_param("order")) {
    auto link =
        (req.has_param("link") && req.get_param_value("link") == "true");
    auto data =
        (req.has_param("data") && req.get_param_value("data") == "true");
    auto order = req.get_param_value("order");
    if (order == "-100") {
      tree->show();
      return;
    }
    delete tree;
    tree = new BPTree(stoi(order));
    cout << "[Init] init B+ Tree with order " << order << endl;
    res.set_content(tree->graphviz(link, data), "text/plain");
    res.status = 200;
  }
}

void bulk(const Request& req, Response& res) {
  auto link = (req.has_param("link") && req.get_param_value("link") == "true");
  auto data = (req.has_param("data") && req.get_param_value("data") == "true");
  auto actions = req.body;
  string line;
  stringstream ss(actions);
  while (getline(ss, line, '\n')) {
    stringstream sl(line);
    string symbol;
    getline(sl, symbol, ' ');
    if (symbol == "+") {
      string key;
      string data;
      getline(sl, key, ',');
      getline(sl, data, ',');
      tree->insert(stoi(key), data);
      cout << "[Insert] insert data \"" << data << "\" with key " << key
           << endl;
    } else if (symbol == "-") {
      string key;
      getline(sl, key, ' ');
      tree->_delete(stoi(key));
      cout << "[Delete] delete key \"" << key << "\"" << endl;
    } else {
      cout << "[Error] unsupported action \"" << symbol << "\"" << endl;
    }
  }
  res.set_content(tree->graphviz(link, data), "text/plain");
}

int main(void) {
  int order;
  cin >> order;
  tree = new BPTree(order);

  string symbol;
  string data;
  while (cin >> symbol) {
    if (symbol == "+") {
      cin >> data;
      stringstream ss(data);
      string key, value;
      getline(ss, key, ',');
      getline(ss, value, ',');
      tree->insert(stoi(key), value);
    } else if (symbol == "-") {
      cin >> data;
      tree->_delete(stoi(data));
    } else {
      break;
    }
  }

  ifstream fin("index.html");

  if (!fin) {
    cout << "[Error] template loading fault: lack index.html" << endl;
    exit(0);
  }

  cout << "GUI server at http://localhost:3000" << endl << endl;
  Server svr;
  svr.set_base_dir("./");
  svr.Get("/", indexPage);
  svr.Get("/graph", getGraph);
  svr.Get("/insert", insert);
  svr.Get("/init", init);
  svr.Get("/delete", _delete);
  svr.Get("/query", query);
  svr.Post("/bulk", bulk);
  svr.Get("/stop", [&](const Request& req, Response& res) {
    cout << "[Quit] bye~" << endl;
    svr.stop();
  });

  svr.listen("localhost", 3000);
}