#ifndef _LEXICAL_H_
#define _LEXICAL_H_

#include <string>


class Token{
    public:
        int type;
        std::string name;
        Token* next;
        Token(): type(0), name(""), next(NULL){}
        Token(int t, std::string n): type(t), name(n), next(NULL){}
};


class LexicalAnalyzer{
    public:
        LexicalAnalyzer(std::string file_path, int print_list);
        void tokenize();

    private:
        int cut_space(int start, std::string line, Token* p_token);
        int cut_identifier(int start, std::string line, Token* p_token);
        int cut_string(int start, std::string line, Token* p_token);
        int cut_comment(int start, std::string line, Token* p_token);
        int cut_operator(int start, std::string line, Token* p_token);
        int cut_int(int start, std::string line, Token* p_token);
        int cut_limiter(int start, std::string line, Token* p_token);
        int paste_to_token_list(int start, std::string line, Token* p_token);
        void do_remove();
        void do_print();


    public:
        Token* head;
        std::string file_path;
        int print_list;
        int line_num;
};

#endif
