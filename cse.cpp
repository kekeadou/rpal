#include "cse.h"
#include "constant.h"


void CSE::generate_control_structs(Tree* st){
    if (st == NULL) return;
    int k = 0;
    Item* item = new Item();
    head = new Control(0);
    switch (st->type){
        case GAMMA: {
            item->type = GAMMA;
            head->delta.push_back(item);
            break;
            }
       case LAMBDA: {
            k++;
            item->type = LAMBDA;
            item->val.l = new leta(k, st->child);
            c0->delta.push_back(item);
            break;
            }

    }
}

Item* CSE::gen_control_struct(Tree* st, int i, Control* cur){
    if (st == NULL) return NULL;
    Item* m = new Item();
    switch(st->type) {
        case ID:
        case INTEGER:
        case STRING:
        case TRUE:
        case FALSE:
        //case XNIL:
        //case YSTAR:
        case DUMMY: {
            m->val.t = st;
            m->next = NULL;
            return m;
            }
        case LAMBDA: {
            i++;
            m->val.l = new leta(i, st->child);
            m->type = LAMBDA;
            cur->next = new Control(i);
            cur = cur->next;
            cur->delta = gen_control_struct(st->sibling, i, cur);
            return m;
            }
    }

}

Item* make_env(Environ* x){
    Item* tmp = new Item();
    tmp->val.epsilon = x;
    return tmp;
}

Environ* CSE::cse_engine(Control* head){
     Environ* e_head=new Environ(0,NULL,NULL,NULL); 
     Item *c_head, *control, *s_head;
     c_head = control = make_env(e_head);
     s_head = make_env(e_head);
     control->next = head->delta;

     while(control->next != NULL) control = control->next;
     switch(control->type) {
        case TUPLE:
        case GENERAL: rule_1(c_head,&s_head); break;
        case LAMBDA: rule_2(c_head,&s_head); break;
        case GAMMA: rule_3_etc(c_head,&s_head,d_head,&enviro); 
            break;
        case ENVIRO: rule_5(c_head,&s_head);break;
        case BINOPA: 
        case BINOPL: rule_6(c_head,&s_head); break;
        case UNOP: rule_7(c_head,&s_head); break;
        case BETA: rule_8(c_head,&s_head); break;
        case TAU: rule_9(c_head,&s_head); break;
        case AUG: rule_6_5(c_head,&s_head); break; 
     }
    return e_head; 
}

void CSE::eval(Tree* st){
    Control* head = new Control(0);
    Control* cur = head;
    head->delta = gen_control_struct(st, 0, cur);
    Environ* e->head = cse_engine(head);

}
