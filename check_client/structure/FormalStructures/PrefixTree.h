//
// Created by Admin on 12.11.2026.
//

#ifndef AWS_HACKATHAN_PREFIXTREE_H
#define AWS_HACKATHAN_PREFIXTREE_H

#include "TrieNode.h"

class PrefixTree {
private:
    TrieNode *root;
public:
    PrefixTree();
    //actions
    void insert(std::string word);
    bool search(std::string word);
    bool starts_with(std::string word);
};



#endif //AWS_HACKATHAN_PREFIXTREE_H
