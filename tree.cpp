#include "constant.h"
#include "tree.h"
#include <iostream>

using namespace std;


void print_tree(Tree* top, int print_dots){
    help_print(top, 0, print_dots);
}

void print_n_dots(int n){
    while(n--) cout << ".";
}

string get_name(Tree* node){
    string s;
    switch (node->type){
        case ID:
            s = "<ID:" + node->name + ">";
            break;
        case INTEGER:
            s = "<INT:" + node->name + ">";
            break;
        case STRING:
            s = "<STR:" + node->name + ">";
            break;
        default:
            s = node->name;
    }
    return s;
}

void help_print(Tree* top, int depth, int per_dots){
    print_n_dots(depth*per_dots);
    string name = get_name(top);
    //cout <<name << " - " << top->type << endl;
    cout << name << endl;

    if (top->child) help_print(top->child, depth+1, per_dots);
    if (top->sibling) help_print(top->sibling, depth, per_dots);
}
