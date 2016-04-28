#include <map>
#include <string>
#include <fstream>
#include <iostream>

#include "constant.h"
#include "lexical.h"

using namespace std;

const map<string, int>::value_type id_init_value[] =
{
    map<string, int>::value_type("let", LET),
    map<string, int>::value_type("in", IN),
    map<string, int>::value_type("lambda", LAMBDA),
    map<string, int>::value_type("fn", FN),
    map<string, int>::value_type("where", WHERE),
    map<string, int>::value_type("tau", TAU),
    map<string, int>::value_type("aug", AUG),
    map<string, int>::value_type("or", OR),
    map<string, int>::value_type("not", NOT),
    map<string, int>::value_type("gr", GR),
    map<string, int>::value_type("ge", GE),
    map<string, int>::value_type("ls", LS),
    map<string, int>::value_type("le", LE),
    map<string, int>::value_type("eq", EQ),
    map<string, int>::value_type("ne", NE),
    map<string, int>::value_type("neg", NEG),
    map<string, int>::value_type("true", TRUE),
    map<string, int>::value_type("false", FALSE),
    map<string, int>::value_type("nil", NIL),
    map<string, int>::value_type("dummy", DUMMY),
    map<string, int>::value_type("within", WITHIN),
    map<string, int>::value_type("and", AND2),
    map<string, int>::value_type("rec", REC),
};

map<string, int> id_name_type_mappings(id_init_value, id_init_value + sizeof(id_init_value)/sizeof(id_init_value[0]));


const map<string, int>::value_type oper_init_value[] =
{
    map<string, int>::value_type(".", PERIOD),
    map<string, int>::value_type("&", AND),
    map<string, int>::value_type("+", ADD),
    map<string, int>::value_type("-", SUB),
    map<string, int>::value_type("*", STAR),
    map<string, int>::value_type("/", DEVIDE),
    map<string, int>::value_type("@", AT_),
    map<string, int>::value_type("|", BAR),
    map<string, int>::value_type("=", EQUALS),
    map<string, int>::value_type("->", CONDITIONAL),
    map<string, int>::value_type("**", TSTAR),
    map<string, int>::value_type(">=", GE),
    map<string, int>::value_type("<=", LE),
};

map<string, int> oper_name_type_mappings(oper_init_value, oper_init_value + sizeof(oper_init_value)/sizeof(oper_init_value[0]));

int get_value_or_default(map<string, int> m, string key, int default_value) {
    if(m.find(key) == m.end()) return default_value;
    return m[key];
}

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
            cout << line << endl;
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
                p_token = new Token(END_FLAG, "end_flag");
                cur->next = p_token; 
            }
            else {
                cout << "Parse Error at line: " << line_num << endl;
                exit(-1);
            }
        }
    }
    do_remove();
    if (print_list)
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
        p_token->name = line.substr(start, end-start);
        p_token->type = get_value_or_default(id_name_type_mappings, p_token->name, ID);
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
            case '*': if(line[end+1] == '*') {
                          p_token->type = TSTAR;
                          p_token->name = "**";
                          return 1;
                      }
            default : {
                          p_token->name = line[end];
                          p_token->type = get_value_or_default(oper_name_type_mappings, p_token->name, OP);
                      }
        }
        //cout << "it is an oper." << endl;
        return 1;
    }
    return 0;
}

int LexicalAnalyzer::cut_int(int start, string line, Token* p_token) {
    int end = start;
    if (isdigit(line[end++])){
        while(isdigit(line[end])) end++;
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
        cur = cur->next;
    }
}

void LexicalAnalyzer::do_print(){
    Token* cur = head;
    while(cur != NULL) {
        cout << "type: " << cur->type << " name: " << cur->name << endl;
        cur = cur->next;
    }
}
