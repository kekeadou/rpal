#ifndef _CSE_H_
#define _CSE_H_

#include <vector>
#include <list>
#include <string>
#include "tree.h"

struct leta {
    int index;
    Tree* x;

    leta(int i, Tree* t): index(i), x(t) {}
};

class Environ;
class Item {
    public:
        int type;
        Item* next;
        union {
            struct leta *l;
            Environ* epsilon;
            Tree* t;
        } val;
};

class Control {
    public:
        int k;
        Item* delta;
        Control* next;

        Control(int ck): k(ck) {}

};

struct next_en { /* Used exclusively within struct environment */
    struct environment *here;
    struct next_en *next;
};


class Environ {
    public:
        int number;
        Environ* prev; 
        struct next_en *next; 
        Item *from;
        Item *to;
        
        Environ(int n, Environ* where, Item* f, Item* t): number(n), prev(where), next(NULL), from(f), to(t){}
};


class CSE {
    public:
        int no_out;
        Control* head;
        Item* gen_control_struct(Tree* st, int index, Control* tmp);
        void eval(Tree* st);
        Environ* rule_engine(Control* head);
        
        void rule_1(Item* c_head, Item** s_head);
        void rule_2(Item* c_head, Item** s_head);
        void rule_3(Item* c_head, Item** s_head, int* env);
        void rule_5(Item* c_head, Item** s_head);
        void rule_6(Item* c_head, Item** s_head);
        void rule_7(Item* c_head, Item** s_head);
        void rule_8(Item* c_head, Item** s_head);
        void rule_9(Item* c_head, Item** s_head);

};


#endif
