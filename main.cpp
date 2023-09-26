#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "./b_plus.hpp"

using namespace std;

BPTree* tree;

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
  tree->show();

}