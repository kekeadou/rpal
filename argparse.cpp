#include <string>
#include <iostream>

#include "argparse.h"

using namespace std;


void arg_parse_error(string error_info){
    cout << "ArgParse Error: " << error_info << endl;
    exit(-1);
}


void ArgParse::parse(int argc, char** argv){
    if(argc == 1) exit(0);
    if (argc == 2) {
        if(argv[1][0] != '-') exit(0);
        cout << "Usage: " << argv[0] << " [-ast] [-l(Optional)] [-noout(Optional)] filename" << endl;
        exit(-1);
    }

    for(int i=1;i<argc;i++) {
       if (strncmp("-l", argv[i], 2) == 0) print_list++;
       else if (strncmp("-ast", argv[i], 4) == 0) print_ast++;
       else if (strncmp("-noout", argv[i], 6) ==0) print_none++;
       else if (argv[i][0] != '-') file_path = string(argv[i]);
    }
}
