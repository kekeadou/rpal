
#include <iostream>

#include "argparse.h"
#include "lexical.h"
#include "syntax.h"

using namespace std;


int main(int argc, char* argv[]){

    ArgParse conf = ArgParse();
    conf.parse(argc, argv);

    LexicalAnalyzer la(conf.file_path, conf.print_list);
    la.tokenize();

    SyntaxAnalyzer sa(la.head, conf.print_ast, conf.print_st);
    sa.build_ast();
    sa.standerdize();
    return 0;
}
