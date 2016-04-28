#ifndef _SYNTAX_H_
#define _SYNTAX_H_

#include "lexical.h"
#include <stack>
#include <list>
#include <string>
#include "tree.h"


class SyntaxAnalyzer{
    private:
        Token* cur;

    public:
        int print_ast;
        int print_st;
        int print_dots;
        std::stack<Tree*> st;
        SyntaxAnalyzer(Token* t): cur(t), print_ast(0), print_st(0),  print_dots(1)  {};
        SyntaxAnalyzer(Token* t, int pa): cur(t), print_ast(pa), print_st(0), print_dots(1) {};
        SyntaxAnalyzer(Token* t, int pa, int ps): cur(t), print_ast(pa), print_st(ps), print_dots(1) {};
        SyntaxAnalyzer(Token* t, int pa, int ps, int pd): cur(t), print_ast(pa), print_st(ps), print_dots(pd) {};


    public:
        void build_ast();
        void standerdize();
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
