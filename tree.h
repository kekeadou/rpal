#ifndef _TREE_H_
#define _TREE_H_

#include <stack>
#include <string>


// left-child, right-sibling tree.
class Tree {
    public:
        int type;
        int value;
        std::string name;
        Tree* child;
        Tree* sibling;
        Tree(int t): type(t), value(0), name(""), child(NULL), sibling(NULL){};
        Tree(int t, std::string n): type(t), value(0), name(n), child(NULL), sibling(NULL){};
        Tree(int t, int v, std::string n): type(t),value(v),name(n), child(NULL), sibling(NULL){};
};

extern std::stack<Tree*> st;

void print_tree(Tree* top, int per_dots);

void help_print(Tree* top, int depth, int per_dots);

#endif /* ifndef _TREE_H_ */
