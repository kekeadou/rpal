#include "constant.h"
#include "syntax.h"
#include <iostream>
#include <map>

using namespace std;

const map<int, string>::value_type init_value[] =
{
    map<int, string>::value_type(LET, "let"),
    map<int, string>::value_type(IN, "in"),
    map<int, string>::value_type(LAMBDA, "lambda"),
    map<int, string>::value_type(FN, "fn"),
    map<int, string>::value_type(WHERE, "where"),
    map<int, string>::value_type(TAU, "tau"),
    map<int, string>::value_type(AUG, "aug"),
    map<int, string>::value_type(OR, "or"),
    map<int, string>::value_type(NOT, "not"),
    map<int, string>::value_type(GR, "gr"),
    map<int, string>::value_type(GE, "ge"),
    map<int, string>::value_type(LS, "ls"),
    map<int, string>::value_type(LE, "le"),
    map<int, string>::value_type(EQ, "eq"),
    map<int, string>::value_type(NE, "ne"),
    map<int, string>::value_type(NEG, "neg"),
    map<int, string>::value_type(TRUE, "true"),
    map<int, string>::value_type(FALSE, "false"),
    map<int, string>::value_type(NIL, "nil"),
    map<int, string>::value_type(DUMMY, "dummy"),
    map<int, string>::value_type(WITHIN, "within"),
    map<int, string>::value_type(AND2, "and"),
    map<int, string>::value_type(REC, "rec"),
    map<int, string>::value_type(PERIOD, "."),
    map<int, string>::value_type(AND, "&"),
    map<int, string>::value_type(ADD, "+"),
    map<int, string>::value_type(SUB, "-"),
    map<int, string>::value_type(STAR, "*"),
    map<int, string>::value_type(DEVIDE, "/"),
    map<int, string>::value_type(AT_, "@"),
    map<int, string>::value_type(BAR, "|"),
    map<int, string>::value_type(EQUALS, "="),
    map<int, string>::value_type(CONDITIONAL, "->"),
    map<int, string>::value_type(TSTAR, "**"),
    map<int, string>::value_type(GAMMA, "gamma"),
    map<int, string>::value_type(COMMA, ","),
    map<int, string>::value_type(FCN_FORM, "fcn_form"),
};

map<int, string> type_name_mappings(init_value, init_value + sizeof(init_value)/sizeof(init_value[0]));

string get_value_or_default(map<int, string> m, int key, string default_value) {
    if(m.find(key) == m.end()) return default_value;
    return m[key];
}

void SyntaxAnalyzer::forward(){
    cur = cur->next;
}

void SyntaxAnalyzer::push_stack_and_forward(){
    Tree* tree_node = new Tree(cur->type, cur->name);
    st.push(tree_node);
    forward();
}

void SyntaxAnalyzer::push_paren(){
    Tree* tree_node = new Tree(EMPTY_PAREN, "<()>");
    st.push(tree_node);
}

int is_type_binary(int type){
    return (type != NOT) && (type!=NEG) && (type!=REC);
}

void SyntaxAnalyzer::build_subtree(int type, int num){
    string name = get_value_or_default(type_name_mappings, type, "NO NAME");
    Tree* new_node = new Tree(type, name);

    stack<Tree*> help_stack;
    for (int i=0; i<num; i++){
        help_stack.push(st.top());
        st.pop();
    }

    new_node->child = help_stack.top();
    help_stack.pop();
    num--;

    Tree* cur_node = new_node->child;
    while(num--){
        cur_node->sibling = help_stack.top();
        help_stack.pop();
        cur_node = cur_node->sibling;
    }
    st.push(new_node);
}

void error_exit(string error_info){
    cout << error_info << endl;
    exit(-1);
}


//Vl -> ’<IDENTIFIER>’ list ’,’ => ’,’?;
int SyntaxAnalyzer::VL() {
    int n = 1;
    if (cur->type == ID){
        push_stack_and_forward();
        while(cur->type==COMMA) {
            forward();
            if (cur->type != ID)
                error_exit("id");
            push_stack_and_forward();
            n++;
        }
        if(n > 1) build_subtree(COMMA, n);
        return 1;
    }
    return 0;
}


//Vb -> ’<IDENTIFIER>’
//   -> ’(’ Vl ’)’
//   -> ’(’ ’)’ => ’()’;
int SyntaxAnalyzer::VB() {
    switch (cur->type) {
        case ID:{
            push_stack_and_forward();
            return 1;
            }
        case L_PAREN: {
            forward();
            if(VL() == 1){
                if (cur->type != R_PAREN) error_exit(") not find.");
                forward();
                return 1;
            }
            if (cur->type != R_PAREN) error_exit(") not find.");
            forward();
            push_paren();
            return 1;
            }
        default : return 0;
    }
}


//Db -> Vl ’=’ E                        => ’=’
//   -> ’<IDENTIFIER>’ Vb+ ’=’ E        => ’fcn_form’
//   -> ’(’ D ’)’ ;
void SyntaxAnalyzer::DB() {
    if (VL() == 1){
        if (cur->type==EQUALS){
            forward();
            E();
            build_subtree(EQUALS, 2);
            return;
        }
    }
    if (cur->type == ID){
        push_stack_and_forward();
        int n = 0;
        while(VB() == 1) n++;
        if(cur->type != EQUALS)
            error_exit("= parse error in DB.");
        forward();
        E();
        build_subtree(FCN_FORM, n+2);
        return;
    }
    if (cur->type == L_PAREN) {
        forward();
        D();
        if(cur->type != R_PAREN)
            error_exit(") not find.");
        forward();
        return;
    }
    error_exit("parse error in DB.");
}


//Dr -> ’rec’ Db
//   -> Db ;
void SyntaxAnalyzer::DR(){
    if (cur->type == REC) {
        forward();
        DB();
        build_subtree(REC, 1);
        return;
    }
    DB();
}


//Da -> Dr ( ’and’ Dr )+
//   -> Dr ;
void SyntaxAnalyzer::DA(){
    DR();
    int n = 1;
    while (cur->type == AND2){
        forward();
        DR();
        n++;
    }
    if(n > 1)
        build_subtree(AND2, n);
}

//D -> Da ’within’ D
//  -> Da ;
void SyntaxAnalyzer::D() {
    DA();
    if (cur->type == WITHIN) {
        forward();
        D();
        build_subtree(WITHIN, 2);
    }
}

//Ew -> T ’where’ Dr => ’where’
//   -> T;
void SyntaxAnalyzer::EW(){
    T();
    if (cur->type == WHERE){
        forward();
        DR();
        build_subtree(WHERE, 2);
    }
}


//E -> ’let’ D ’in’ E => ’let’
//  -> ’fn’ Vb+ ’.’ E => ’lambda’
//  -> Ew;
void SyntaxAnalyzer::E(){
    if (cur->type == LET) {
        forward();
        D();
        if(cur->type != IN)
            error_exit("no 'in' after let in E().");
        forward();
        E();
        build_subtree(LET, 2);
        return;
    }
    if (cur->type == FN) {
        forward();
        int n=0;
        while(cur->type != PERIOD) {
            n++;
            VB();
        }
        forward();
        E();
        if (n == 0) build_subtree(LAMBDA, 2);
        else build_subtree(LAMBDA, n+1);
        return ;
    }
    EW();
}


//Tc -> B ’->’ Tc ’|’ Tc => ’->’
//   -> B
void SyntaxAnalyzer::TC() {
    B();
    if(cur->type == CONDITIONAL) {
        forward();
        TC();
        if(cur->type != BAR) error_exit("no | find in TC.");
        forward();
        TC();
        build_subtree(CONDITIONAL, 3);
    }
}


//Ta -> Ta ’aug’ Tc => ’aug’
//   -> Tc ;
void SyntaxAnalyzer::TA(){
    TC();
    while(cur->type == AUG) {
        forward();
        TC();
        build_subtree(AUG, 2);
    }
}


//T -> Ta ( ’,’ Ta )+ => ’tau’
//  -> Ta ;
void SyntaxAnalyzer::T(){
    TA();
    int n = 1;
    while(cur->type == COMMA){
        forward();
        TA();
        n++;
    }
    if(n > 1)
        build_subtree(TAU, n);
}


//Bp -> A (’gr’ | ’>’ ) A => ’gr’
//-> A (’ge’ | ’>=’) A => ’ge’
//-> A (’ls’ | ’<’ ) A => ’ls’
//-> A (’le’ | ’<=’) A => ’le’
//-> A ’eq’ A => ’eq’
//-> A ’ne’ A => ’ne’
//-> A ;
void SyntaxAnalyzer::BP() {
    A();
    switch(cur->type){
        case GE:
        case LS:
        case LE:
        case EQ:
        case NE: {
                int this_type = cur->type;
                forward();
                A();
                build_subtree(this_type, 2);
            }
        default: return;
    }
}

//Bs -> ’not’ Bp => ’not’
//   -> Bp ;
void SyntaxAnalyzer::BS(){
    if(cur->type == NOT){
        forward();
        BP();
        build_subtree(NOT, 1);
        return;
    }
    BP();
}


//Bt -> Bt ’&’ Bs => ’&’
//   -> Bs ;
void SyntaxAnalyzer::BT(){
    BS();
    while(cur->type == AND) {
        forward();
        BS();
        build_subtree(AND, 2);
    }
}


//B ->B’or’ Bt => ’or’
//  -> Bt ;
void SyntaxAnalyzer::B(){
    BT();
    while (cur->type == OR){
        forward();
        BT();
        build_subtree(OR, 2);
    }
}


//Ap -> Ap ’@’ ’<IDENTIFIER>’ R => ’@’
//   -> R ;
void SyntaxAnalyzer::AP(){
    R();
    while(cur->type == AT_){
        forward();
        if(cur->type != ID) error_exit("ID not find in AP");
        push_stack_and_forward();
        R();
        build_subtree(AT_, 3);
    }
}

//Af -> Ap ’**’ Af => ’**’
//-> Ap ;
// TODO
void SyntaxAnalyzer::AF(){
    AP();
    int n = 0;
    while (cur->type == TSTAR){
        n++;
        forward();
        AP();
    }
    while (n-- > 0) {
        build_subtree(TSTAR, 2);
    }
}

//At -> At ’*’ Af => ’*’
//-> At ’/’ Af => ’/’
//-> Af ;
void SyntaxAnalyzer::AT(){
    AF();
    while ((cur->type == STAR) || (cur->type == DEVIDE)){
        int this_type = cur->type;
        forward();
        AF();
        build_subtree(this_type, 2);
    }
}


//A ->A’+’ At => ’+’
//-> A ’-’ At => ’-’
//-> ’+’ At
//-> ’-’ At => ’neg’
//-> At ;
void SyntaxAnalyzer::A(){
    if (cur->type == ADD){
        forward();
        AT();
        return;
    }
    if (cur->type == SUB){
        forward();
        AT();
        build_subtree(NEG, 1);
        return;
    }
    AT();
    while ((cur->type == ADD) or (cur->type == SUB)){
        int this_type = cur->type;
        forward();
        A();
        build_subtree(this_type, 2);
    }
}

//Rn -> ’<IDENTIFIER>’
//-> ’<INTEGER>’
//-> ’<STRING>’
//-> ’true’ => ’true’
//-> ’false’ => ’false’
//-> ’nil’ => ’nil’
//-> ’(’ E ’)’
//-> ’dummy’ => ’dummy’ ;
int SyntaxAnalyzer::RN() {
    switch(cur->type){
        case ID:
        case INTEGER:
        case STRING:
        case TRUE:
        case FALSE:
        case NIL:
        case DUMMY:{
            push_stack_and_forward();
            return 1;
        }
        case L_PAREN:{
                forward();
                E();
                if(cur->type != R_PAREN) error_exit(") not find in RN");
                forward();
                return 1;
            }
    }
    return 0;
}

//R ->R Rn => ’gamma’
//-> Rn ;
void SyntaxAnalyzer::R(){
    if(RN() == 1){
        while(RN() == 1){
            build_subtree(GAMMA, 2);
        }
    }
}


void SyntaxAnalyzer::build_ast(){
    E();
    if (cur->next != NULL){
        cout << cur->type << "-" << cur->name << "-" << endl;
        error_exit("can not parse tokens totally.");
    }

    if (print_ast){
        print_stack(); 
    }
}

void standerdize_node(Tree* node){
    switch (node->type) {
        case LET:
            {
                if (node->child->type != EQUALS) break;
                Tree* tmp = node->child->child->sibling;
                node->child->child->sibling = node->child->sibling;
                node->child->sibling = tmp;
                node->child->type=LAMBDA;
                node->child->name="lambda";
                node->type=GAMMA;
                node->name="gamma";
                break;
            }
        case WHERE:
            {
                cout << node->child->child->name << endl;
                if(node->child->sibling->type != EQUALS) break;
                Tree* P = node->child;
                Tree* E = node->child->sibling->child->sibling;
                node->child->sibling->child->sibling = P;
                node->child = node->child->sibling;
                node->child->sibling = E;
                node->child->type=LAMBDA;
                node->child->name="lambda";
                node->type=GAMMA;
                node->name="gamma";
                cout << node->child->name << node->child->sibling->name << endl;
                cout << node->child->child->name << node->child->child->sibling->name << endl;
                break; 
            }
        case WITHIN: {
           if( !((node->child->type==EQUALS) &&
                (node->child->sibling->type==EQUALS))) break; 
            Tree* E2 = node->child->sibling->child->sibling;
            Tree* X2 = node->child->sibling->child;
            node->child->sibling->child = new Tree(0);
            node->child->sibling->child->sibling = node->child->child->sibling;
            node->child = X2;
            node->child->sibling->child->child->sibling = E2;
            node->type = EQUALS;
            node->child->sibling->type = GAMMA;
            node->child->sibling->child->type = LAMBDA;
            break;
            }
    }

}

void do_standerdize(Tree* top){
    if (!top) return;
    standerdize_node(top);
    if (top->child) do_standerdize(top->child);
    if (top->sibling) do_standerdize(top->sibling);
}

void SyntaxAnalyzer::standerdize(){
    Tree* top = st.top();
    do_standerdize(top);

    if (print_st){
        print_stack(); 
    }
}

void SyntaxAnalyzer::debug(){
    cout << cur->type << " - " << cur->name << endl;
    print_stack();
}

void SyntaxAnalyzer::print_stack(){
    Tree* top = st.top();
    //cout << top->name << " - " << endl;
    print_tree(top, print_dots);
    //st.pop();
}

