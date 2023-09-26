

#include <string>
#include <vector>
using namespace std;
#define uint unsigned int
enum NodeType { SearchNode, DataNode };

class SNode;
class BPTree;
class DPTree;

class Data {
 public:
  int key;
  vector<string> values;
  Data(int, string);
  string dumpValue();
  void addValue(string);
};

class Node {
 protected:
  NodeType type;
  uint order = 0;
  BPTree* tree;
  friend class BPTree;

 public:
  uint pIndex = 0;  //記錄上層節點的位置
  int parent = -1;  //記錄上層節點
  int id;
  inline uint getSize();
  virtual ~Node() = default;
  void setParent(SNode*, uint);
  SNode* getParent();
};

/*  搜尋用節點 */
class SNode : public Node {
 private:
  friend class BPTree;
  friend class DNode;

 public:
  uint kCount = 0;
  int* keys;
  Node** ptrs;
  uint pCount = 0;
  void newKey(uint);
  SNode(BPTree*);
  Node* search(int);
  void relateNode(int, Node*);  // by key
  void removeKey(uint);         // by index
};

/*  資料節點 */
class DNode : public Node {
 private:
  Data** data;     //資料本體
  int dCount = 0;  //資料數量
  int lNode = -1;
  int rNode = -1;
  friend class BPTree;

  bool insert(Data*);
  bool _delete(int);

 public:
  DNode(BPTree*);
};

class BPTree {
 private:
  uint order;
  uint nCount = 0;  // 總節點數
  Node* root;
  friend class SNode;
  friend class DNode;
  void printOut(Node*, uint, string);

 public:
  vector<Node*> nodes;
  BPTree(uint);
  void insert(int, string);
  void _delete(int);
  void addNode(Node*);
  void show();
  string graphviz(bool, bool);
  Data* query(int key);
  int getCount();
};
