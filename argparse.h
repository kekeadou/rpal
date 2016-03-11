#ifndef _ARGPARSE_H_
#define _ARGPARSE_H_

#include <string>


class ArgParse{
    public:
        void parse(int argc, char** argv);

    public:
        int print_list;
        int print_ast;
        int print_none;
        std::string file_path;
};

#endif
