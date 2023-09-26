#include "./b_plus.hpp"

#include <algorithm>
#include <iostream>
using namespace std;
#define DEBUG if (0)

class SNode;
class BPTree;

Data::Data(int _key, string _value) {
  key = _key;
  values.push_back(_value);
}

void Data::addValue(string value) { values.push_back(value); }

string Data::dumpValue() {
  string res = values[0];
  for (int i = 1; i < values.size(); i++) res += ", " + values[i];
  return res;
}

inline uint Node::getSize() { return order * 2; }
void Node::setParent(SNode* _parent, uint _index) {
  parent = _parent->id;
  pIndex = _index;
}

void SNode::removeKey(uint index) {
  for (int i = index; i < (int)kCount - 1; i++) {
    keys[i] = keys[i + 1];
    ptrs[i + 1] = ptrs[i + 2];
    ptrs[i + 1]->pIndex = i + 1;
  }
  kCount--;
  pCount--;
  if (kCount == 0 && parent == -1) {
    tree->root = ptrs[0];
    ptrs[0]->parent = -1;
    tree->nodes[id] = nullptr;
    delete this;
  } else if (kCount < order && parent != -1) {
    SNode* _parent = dynamic_cast<SNode*>(tree->nodes[parent]);

    if (pIndex > 0) {
      int combineKey = _parent->keys[pIndex - 1];
      SNode* lNode = dynamic_cast<SNode*>(_parent->ptrs[pIndex - 1]);
      if (lNode->kCount <= lNode->getSize() - 2) {
        DEBUG cout << "<<<<way1 L>>>>" << endl;
        int i = 0;

        lNode->keys[lNode->kCount++] = combineKey;

        for (i = 0; i < kCount; i++) {
          lNode->keys[lNode->kCount++] = keys[i];
          lNode->ptrs[lNode->pCount++] = ptrs[i];
          ptrs[i]->parent = lNode->id;
        }
        lNode->ptrs[lNode->pCount++] = ptrs[i];
        ptrs[i]->parent = lNode->id;
        tree->nodes[id] = nullptr;
        _parent->removeKey(pIndex - 1);
        SNode* node = _parent;

      } else {
        DEBUG cout << "<<<<way2 L>>>>>>>>" << endl;
        newKey(-1);
        keys[0] = combineKey;
        ptrs[0] = lNode->ptrs[lNode->pCount - 1];
        ptrs[0]->pIndex = 0;
        lNode->ptrs[lNode->pCount - 1]->parent = id;
        _parent->keys[pIndex - 1] = lNode->keys[lNode->kCount - 1];
        lNode->kCount--;
        lNode->pCount--;
      }

    } else if (pIndex < _parent->pCount - 1) {
      SNode* rNode = dynamic_cast<SNode*>(_parent->ptrs[pIndex + 1]);

      int combineKey = _parent->keys[pIndex];
      if (rNode->kCount <= rNode->getSize() - 2) {
        DEBUG cout << "<<<<way1 R>>>>" << endl;
        int i = 0;

        keys[kCount++] = combineKey;

        for (i = 0; i < rNode->kCount; i++) {
          keys[kCount++] = rNode->keys[i];
          ptrs[pCount++] = rNode->ptrs[i];
          rNode->ptrs[i]->parent = id;
        }
        ptrs[pCount++] = rNode->ptrs[i];
        rNode->ptrs[i]->parent = id;

        tree->nodes[rNode->id] = nullptr;
        _parent->removeKey(pIndex);
        SNode* node = _parent;
      } else {
        DEBUG cout << "<<<<way2 R>>>>>>>>" << endl;
        keys[kCount] = combineKey;
        ptrs[pCount] = rNode->ptrs[0];
        rNode->ptrs[0]->parent = id;
        _parent->keys[pIndex] = rNode->keys[0];
        rNode->ptrs[0] = rNode->ptrs[1];
        rNode->ptrs[0]->pIndex = 0;
        rNode->removeKey(0);
        kCount++;
        pCount++;
      }
    }
  }
}

void SNode::newKey(uint index) {
  for (int i = (int)kCount - 1; i >= (int)index; i--) {
    keys[i + 1] = keys[i];
  }
  DEBUG cout << "pCount of " << id << ": " << pCount << endl;
  for (int i = (int)pCount - 1; i >= (int)index + 1; i--) {
    ptrs[i + 1] = ptrs[i];
    ptrs[i + 1]->pIndex = i + 1;
  }
  kCount++;
  pCount++;
}

SNode* Node::getParent() { return dynamic_cast<SNode*>(tree->nodes[parent]); }

Node* SNode::search(int value) {
  int i;
  for (i = 0; i < kCount; i++) {
    if (value < keys[i]) return ptrs[i];
  }
  return ptrs[i];
}

bool DNode::insert(Data* _value) {
  DEBUG cout << "internal_insert1::" << getSize() << endl;
  DEBUG if (parent != -1) cout << "My parent::" << parent << endl;

  Data* value = _value;
  uint index = 0;
  while (index < dCount) {
    DEBUG cout << "internal/" << index << endl;
    if (data[index]->key == _value->key) {
      data[index]->addValue(_value->values[0]);
      return dCount > getSize();
    } else if (data[index]->key > value->key) {
      DEBUG cout << "get" << index << endl;
      Data* swap = data[index];
      data[index] = value;
      value = swap;
    }
    index++;
  }
  DEBUG cout << "dCount/" << dCount << endl;
  data[index] = value;
  dCount++;
  if (dCount > getSize()) {
    int mid = (dCount / 2);
    if (parent == -1) {  // 自己為root
      DEBUG cout << "I'm the root of the tree!!" << endl;

      DEBUG cout << "clear" << endl;

      SNode* sNode = new SNode(tree);
      DNode* n_dNode1 = new DNode(tree);
      DNode* n_dNode2 = new DNode(tree);
      n_dNode1->setParent(sNode, 0);
      n_dNode2->setParent(sNode, 1);
      n_dNode1->rNode = n_dNode2->id;
      n_dNode2->lNode = n_dNode1->id;

      for (int i = 0; i < mid; i++) {
        n_dNode1->insert(data[i]);
      }

      for (int i = mid; i < dCount; i++) {
        n_dNode2->insert(data[i]);
      }

      DEBUG cout << "data" << endl;

      sNode->keys[0] = data[mid]->key;
      sNode->kCount++;
      sNode->ptrs[0] = n_dNode1;
      sNode->ptrs[1] = n_dNode2;
      sNode->pCount = 2;

      tree->nodes[id] = nullptr;

      tree->root = sNode;
      DEBUG cout << "finished" << endl;
      delete this;
    } else {
      DEBUG cout << "I'm not the root!! " << parent << endl;
      SNode* _parent = getParent();

      DNode* n_dNode = new DNode(tree);

      for (int i = mid; i < dCount; i++) n_dNode->insert(data[i]);
      dCount = mid;
      DEBUG cout << "Origin to " << lNode << "|" << rNode << endl;

      if (rNode != -1) {
        dynamic_cast<DNode*>(tree->nodes[rNode])->lNode = n_dNode->id;
        n_dNode->rNode = rNode;
      }
      n_dNode->lNode = id;
      rNode = n_dNode->id;
      DEBUG cout << "split to " << id << "|" << n_dNode->id << endl;
      DEBUG cout << lNode << "|" << rNode << "|" << n_dNode->lNode << "|"
                 << n_dNode->rNode << endl;
      _parent->relateNode(data[mid]->key, n_dNode);
    }
  }

  return false;
}

bool DNode::_delete(int key) {
  int i;
  bool flag = false;
  for (i = 0; i < dCount; i++)
    if (data[i]->key == key) {
      flag = true;
      break;
    }

  for (; i < dCount - 1; i++) {
    data[i] = data[i + 1];
  }
  if (flag) dCount--;
  return dCount < order;
}

BPTree::BPTree(uint _order) {
  order = _order;
  DNode* node = new DNode(this);
  root = node;
  nCount = 1;
  nodes = vector<Node*>();
  nodes.push_back(root);
}

Data* BPTree::query(int key) {
  Node* current = root;
  while (current->type == SearchNode) {
    SNode* sNode = dynamic_cast<SNode*>(current);
    current = sNode->search(key);
  }

  DNode* dNode = dynamic_cast<DNode*>(current);
  for (int i = 0; i < dNode->dCount; i++) {
    if (dNode->data[i]->key == key) return dNode->data[i];
  }
  return nullptr;
}

void BPTree::printOut(Node* node, uint level = 1, string prefix = "") {
  for (int j = 0; j < level - 1; j++) cout << "  ";
  if (node->type == SearchNode) {
    SNode* sNode = dynamic_cast<SNode*>(node);
    cout << prefix << ": [";
    for (int i = 0; i < sNode->kCount; i++) {
      if (i > 0) cout << ",";
      cout << sNode->keys[i];
    }
    cout << "]" << endl;
    for (int i = 0; i < sNode->pCount; i++) {
      printOut(sNode->ptrs[i], level + 1, prefix + "." + to_string(i + 1));
    }
  } else {
    DNode* rNode = dynamic_cast<DNode*>(node);
    cout << prefix << ": [";
    for (int i = 0; i < rNode->dCount; i++) {
      if (i > 0) cout << ",";
      cout << rNode->data[i]->key;
    }
    DEBUG {
      cout << "] parent:" << rNode->parent << " pIndex: " << rNode->pIndex
           << " l< " << rNode->lNode << "|" << rNode->rNode << " >r" << endl;
    }
    else {
      cout << "]" << endl;
    }

    for (int i = 0; i < rNode->dCount; i++) {
      for (int j = 0; j < level; j++) cout << "  ";
      cout << rNode->data[i]->key << ": " << rNode->data[i]->dumpValue()
           << endl;
    }
  }
}

void BPTree::insert(int key, string value) {
  Node* current = root;
  while (current->type == SearchNode) {
    SNode* sNode = dynamic_cast<SNode*>(current);
    current = sNode->search(key);
  }

  DNode* dNode = dynamic_cast<DNode*>(current);
  dNode->insert(new Data(key, value));
}

void BPTree::_delete(int key) {
  Node* current = root;
  while (current->type == SearchNode) {
    SNode* sNode = dynamic_cast<SNode*>(current);
    current = sNode->search(key);
  }

  DNode* dNode = dynamic_cast<DNode*>(current);

  if (dNode->_delete(key) && dNode->parent != -1) {
    DNode* nodeL;
    DNode* nodeR;
    if (dNode->lNode > -1 && dNode->parent == nodes[dNode->lNode]->parent) {
      nodeL = dynamic_cast<DNode*>(nodes[dNode->lNode]);
      nodeR = dNode;
    } else if (dNode->rNode > -1 &&
               dNode->parent == nodes[dNode->rNode]->parent) {
      nodeL = dNode;
      nodeR = dynamic_cast<DNode*>(nodes[dNode->rNode]);
    }
    SNode* _parent = dynamic_cast<SNode*>(nodes[nodeL->parent]);

    DEBUG {
      for (int i = 0; i < _parent->pCount; i++)
        cout << _parent->ptrs[i]->id << "," << endl;

      cout << endl;
      cout << "L:" << nodeL->pIndex << "R:" << nodeR->pIndex << endl;
      cout << "remove:" << _parent->keys[nodeL->pIndex]
           << "index:" << nodeL->pIndex << endl;
    }

    _parent->removeKey(nodeL->pIndex);

    DEBUG for (int i = 0; i < _parent->pCount; i++) cout << _parent->ptrs[i]->id
                                                         << "," << endl;

    nodeL->rNode = nodeR->rNode;
    if (nodeR->rNode != -1)
      dynamic_cast<DNode*>(nodes[nodeR->rNode])->lNode = nodeL->id;

    for (int i = 0; i < nodeR->dCount; i++) {
      for (int j = 0; j < nodeR->data[i]->values.size(); j++) {
        insert(nodeR->data[i]->key, nodeR->data[i]->values[j]);
      }
    }

    delete nodeR;
    nodes[nodeR->id] = nullptr;
  }
}

void BPTree::show() {
  SNode* sNode = dynamic_cast<SNode*>(root);
  printOut(root, 1, "1");
}

string BPTree::graphviz(bool showLink = false, bool showData = false) {
  string graph = "digraph g {node [shape = record,height=.1];\n";

  for (int i = 0; i < nodes.size(); i++) {
    if (nodes[i] == nullptr) continue;

    string label = "";
    if (nodes[i]->type == SearchNode) {
      SNode* sNode = dynamic_cast<SNode*>(nodes[i]);
      for (int j = 0; j < sNode->getSize(); j++) {
        label += "<f" + to_string(j) + ">." + "|" +
                 (j < sNode->kCount ? to_string(sNode->keys[j]) : "") + "|";
      }
      label += "<f" + to_string(sNode->getSize()) + ">.";
      graph +=
          "node" + to_string(nodes[i]->id) + "[label = \"" + label + "\"];\n";
    } else {
      DNode* dNode = dynamic_cast<DNode*>(nodes[i]);
      if (nodes[i] == nullptr) continue;
      if (dNode->dCount == 0) {
        label += "-";
      } else {
        for (int j = 0; j < dNode->dCount - 1; j++) {
          label += to_string(dNode->data[j]->key);
          if (showData) label += " = " + dNode->data[j]->dumpValue();
          label += "|";
        }
        label += to_string(dNode->data[dNode->dCount - 1]->key);
        if (showData)
          label += " = " + dNode->data[dNode->dCount - 1]->dumpValue();
      }
      graph += "node" + to_string(nodes[i]->id) + "[label = \"<start>.|" +
               label + "|<end>.\"];\n";
    }
  }
  graph += "edge [style=\" dashed \"];\n";
  for (int i = 0; i < nodes.size(); i++) {
    if (nodes[i] == nullptr) continue;
    if (nodes[i]->type == SearchNode) {
      SNode* sNode = dynamic_cast<SNode*>(nodes[i]);
      for (int j = 0; j < sNode->pCount; j++) {
        graph += "\"node" + to_string(i) + "\":f" + to_string(j) +
                 " -> \"node" + to_string(sNode->ptrs[j]->id) + "\";\n";
      }
    } else if (showLink) {
      DNode* dNode = dynamic_cast<DNode*>(nodes[i]);
      if (dNode->lNode != -1) {
        graph += "\"node" + to_string(i) + "\":start" + " -> \"node" +
                 to_string(dNode->lNode) + "\":end;\n";
      }
      if (dNode->rNode != -1) {
        graph += "\"node" + to_string(i) + "\":end" + " -> \"node" +
                 to_string(dNode->rNode) + "\":start;\n";
      }
    }
  }

  return graph + "}\n";
}

void BPTree::addNode(Node* node) { nodes.push_back(node); }

int BPTree::getCount() { return nodes.size(); }

SNode::SNode(BPTree* _tree) {
  id = _tree->getCount();
  DEBUG cout << "New SNode:" << id << endl;
  type = SearchNode;
  order = _tree->order;
  keys = new int[getSize() + 1];  //多出的一個為暫存用
  ptrs = new Node*[getSize() + 2];
  tree = _tree;
  _tree->addNode(this);
}

void SNode::relateNode(int key, Node* _node) {
  DEBUG cout << "pCount of " << id << ": " << pCount << endl;
  DEBUG cout << "relateNode " << key << endl;
  int index;
  for (index = 0; index < kCount; index++)
    if (key < keys[index]) break;

  DEBUG cout << "New Key At:" << index << " / For: " << key << endl;
  newKey(index);
  keys[index] = key;
  ptrs[index + 1] = _node;
  DEBUG {
    cout << "updated Keys:";
    for (int i = 0; i < kCount; i++) {
      cout << keys[i] << " ";
    }
    cout << endl;
  }

  _node->setParent(this, index + 1);

  if (kCount == getSize() + 1) {
    DEBUG cout << "overflow" << endl;
    int mid = (int)kCount / 2;
    int splitKey = keys[mid];

    DEBUG cout << "splitKey:" << splitKey << endl;

    DEBUG for (int i = 0; i < kCount; i++) cout << keys[i] << endl;

    SNode* n_sNode = new SNode(tree);  // 用於分資料的新節點
    for (int i = 0; i < mid; i++) n_sNode->keys[i] = keys[mid + 1 + i];
    n_sNode->kCount = mid;
    kCount = mid;
    for (int i = 0; i < mid + 1; i++) {
      n_sNode->ptrs[i] = ptrs[mid + 1 + i];
      n_sNode->ptrs[i]->setParent(n_sNode, i);
      DEBUG cout << "Checked:" << n_sNode->ptrs[i]->id << "/"
                 << n_sNode->ptrs[i]->parent << endl;
    }
    DEBUG {
      if (mid + 1 != (int)(pCount / 2))
        cout << mid + 1 << " !!!!!!!!!!!!!!!!!!!!! " << pCount / 2 << endl;
    }
    n_sNode->pCount = mid + 1;
    pCount = mid + 1;

    if (parent == -1) {                  //  自身為root
      SNode* p_sNode = new SNode(tree);  // 上層節點
      p_sNode->keys[0] = splitKey;
      p_sNode->kCount = 1;
      p_sNode->ptrs[0] = this;
      setParent(p_sNode, 0);
      p_sNode->ptrs[1] = n_sNode;
      n_sNode->setParent(p_sNode, 1);
      p_sNode->pCount = 2;
      tree->root = p_sNode;
    } else {
      getParent()->relateNode(splitKey, n_sNode);
    }
  }
}

DNode::DNode(BPTree* _tree) {
  id = _tree->getCount();
  DEBUG cout << "New DNode:" << id << endl;
  type = DataNode;
  order = _tree->order;
  data = new Data*[getSize() + 1];  //多出的一個為暫存用
  tree = _tree;
  _tree->addNode(this);
}