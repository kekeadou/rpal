

void standerdize(Tree* root){
    try_standerdize(root);
    for(list<Tree*>::iterator it=root->lt.begin(); it != top->lt.end(); ++it) {
        if(*it != NULL) standerdize(*it); 
    }
}
