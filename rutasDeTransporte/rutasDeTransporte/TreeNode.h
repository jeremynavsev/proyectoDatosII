#ifndef TREENODE_H
#define TREENODE_H

#include "Station.h"

class TreeNode {
public:
    Station data;
    TreeNode* left;
    TreeNode* right;
    
    TreeNode(const Station& station);
    ~TreeNode();
};

#endif // TREENODE_H
