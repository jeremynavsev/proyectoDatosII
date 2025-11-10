#include "TreeNode.h"

TreeNode::TreeNode(const Station& station) 
    : data(station), left(nullptr), right(nullptr) {}

TreeNode::~TreeNode() {}
