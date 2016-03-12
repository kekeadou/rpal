#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include "lexical.h"
#include <stack>
#include <list>
#include <string>


class Tree {
    public:
        int type;
        int value;
        std::string name;
        std::list<Tree*> lt;
        Tree(int t): type(t), value(0), name(""){};
        Tree(int t, std::string n): type(t), value(0), name(n){};
        Tree(int t, int v, std::string n): type(t),value(v),name(n){};
};


class SyntaxAnalyzer{

    public:
        Token* cur;
        int print_ast;
        int print_dots;
        std::stack<Tree*> st;
        SyntaxAnalyzer(Token* t): cur(t), print_ast(0), print_dots(3)  {};
        SyntaxAnalyzer(Token* t, int pa): cur(t), print_ast(pa), print_dots(3) {};
        SyntaxAnalyzer(Token* t, int pa, int pd): cur(t), print_ast(pa), print_dots(pd) {};


    public:
        void build_ast();
        void print_stack();
        void debug();

    private:
        void forward();
        void push_stack_and_forward();
        void push_paren();
        void build_subtree(int type, int num);

        int VL();
        int VB();
        void DB();
        void DR();
        void DA();
        void D();
        void EW();
        void E();
        void TC();
        void TA();
        void T();
        void BP();
        void BS();
        void BT();
        void B();
        void AP();
        void AF();
        void AT();
        void A();
        int RN();
        void R();
};


#endif
