#include "constant.h"
#include "lexical.h"
#include <fstream>
#include <iostream>

using namespace std;

int isoperator(char t){
    return  string("\"+-*<>&.@/:=~|$!#%^_[]{}`?").find(t) != string::npos;
}

int ispunc(char t){
    return string("();,").find(t) != string::npos;
}

int is_valid_escape(int t, string line){
    int tmp = t;
    if (line[tmp] == '\\'){
        if(tmp < line.length()-1){
            switch (line[tmp+1]) {
                case '\\' :
                case '\'':
                case 't':
                case 'n': break;
                default: return 0;
            }
        }
        return 1;
    }
    return 0;
}

LexicalAnalyzer::LexicalAnalyzer(string fp, int pl) {
    file_path = fp;
    print_list = pl;
    line_num = 0;
    head = NULL;
}

void LexicalAnalyzer::tokenize() {
    ifstream input(file_path);
    string line;
    head = new Token();
    Token* p_token = head;
    Token* cur;
    while(getline(input, line)) {
        if(print_list)
            cout << line << " ";
        line_num++;
        //cout << line_num << " " << endl;

        int i=0;
        while(i < line.length()) {
            cur = p_token;
            if(cut_space(i, line, p_token) ||
                cut_identifier(i, line, p_token) ||
                cut_string(i, line, p_token) ||
                cut_comment(i, line, p_token) ||
                cut_operator(i, line, p_token) ||
                cut_int(i, line, p_token) ||
                cut_limiter(i, line, p_token)){
                //cout << "--" << i << " " << p_token->name << endl;
                //cout << p_token->name << " " << endl;
                i += p_token->name.length();
                p_token = new Token();
                cur->next = p_token;
            }
            else {
                cout << "Parse Error at line: " << line_num << endl;
                exit(-1);
            }
        }
        cout << endl;
    }
    do_remove();
    do_print();
}

int LexicalAnalyzer::cut_space(int start, string line, Token* p_token){
    int end = start;
    if (isspace(line[end])){
        while(isspace(line[++end]));
        p_token->type = REMOVE;
        p_token->name = line.substr(start, end-start);
        return 1;
    }
    return 0;
}

int LexicalAnalyzer::cut_identifier(int start, string line, Token* p_token) {
    int end=start;
    if(isalpha(line[end++])){
        while(isalnum(line[end]) || line[end] == '_') end++;
        p_token->type = ID;
        p_token->name = line.substr(start, end-start);
        return 1;
    }
    return 0;
}

int LexicalAnalyzer::cut_string(int start, string line, Token* p_token) {
    int end = start;
    if (line[end++] == '\'') {
        while(isalnum(line[end]) || isspace(line[end]) || isoperator(line[end]) || ispunc(line[end]) || is_valid_escape(end, line)) end++;
        if (line[end] == '\'') {
            p_token->type = STRING;
            p_token->name = line.substr(start, end-start+1);
            //*p_p_token = end-start+1;
            return 1;
        }
    }
    return 0;
}

int LexicalAnalyzer::cut_comment(int start, string line, Token* p_token) {
    int end = start;
    if (line.substr(end, 2) == "//") {
        p_token->type = REMOVE;
        p_token->name = line.substr(end+2, line.length()-end-1);
        //*p_p_token = line.length()-1;
        return 1;
    }
    return 0;
}

int LexicalAnalyzer::cut_operator(int start, string line, Token* p_token) {
    int end = start;
    if(isoperator(line[end])) {
        switch(line[end]) {
            case '>': if(line[end+1] == '='){
                          p_token->type = GE;
                          p_token->name = "ge";
                          return 1;
                      }
                      else{
                          p_token->type = GR;
                          p_token->name = "gr";
                          return 1;
                      }
            case '<': if(line[end+1] == '='){
                        //# type is oper, <=
                          p_token->type = LE;
                          p_token->name = "le";
                          return 1;
                      }
                      else {
                          p_token->type = LS;
                          p_token->name = "ls";
                          return 1;
                      }
            case '-': if(line[end+1] == '>') {
                          p_token->type = CONDITIONAL;
                          p_token->name = "->";
                          return 1;
                      }
                      break;
            case '*': if(line[end+1] == '*') {
                          p_token->type = TSTAR;
                          p_token->name = "**";
                          return 1;
                      }
                      break;
        }
        p_token->type = OP;
        p_token->name = line[end];
        //cout << "it is an oper." << endl;
        return 1;
    }
    return 0;
}

int LexicalAnalyzer::cut_int(int start, string line, Token* p_token) {
    int end = start;
    if (isdigit(line[end++])){
        while(isdigit(line[end++]));
        p_token->type = INTEGER;
        p_token->name = line.substr(start, end-start);
        //*p_p_token = end-start;
        return 1;
    }
    return 0;
}

int LexicalAnalyzer::cut_limiter(int start, string line, Token* p_token) {
    int end = start;
    if(ispunc(line[end])) {
        switch(line[end]) {
            case '(': {p_token->type=L_PAREN; p_token->name="(";break;}
            case ')': {p_token->type=R_PAREN; p_token->name=")";break;}
            case ';': {p_token->type=SEMI; p_token->name=";";break;}
            case ',': {p_token->type=COMMA; p_token->name=",";break;}
            default: cout << "unrecognized limiter.";
        }
        return 1;
    }
    return 0;
}

void LexicalAnalyzer::do_remove(){
    while (head->type == REMOVE){
        head = head->next;
    }
    Token* cur = head;
    while(cur->next != NULL) {
        while(cur->next->type==REMOVE) {
            Token* remove = cur->next;
            cur->next = remove->next;
            free(remove);
        } 
        if (cur != NULL)
            cout << cur->type << " " << cur->name << endl; 
            cur = cur->next;
    }
    cout << "end";
}

void LexicalAnalyzer::do_print(){
    Token* cur = head;
    while(cur != NULL) {
        cout << cur->type << " " << cur->name << endl; 
        cur = cur->next;
    }
}
