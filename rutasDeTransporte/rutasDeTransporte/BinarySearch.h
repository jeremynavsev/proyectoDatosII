#ifndef BINARYSEARCH_H
#define BINARYSEARCH_H

#include "TreeNode.h"
#include "Station.h"
#include <QVector>
#include <QString>

/**
 * @brief Binary Search Tree for storing stations
 */
class BinarySearchTree {
public:
    BinarySearchTree();
    ~BinarySearchTree();
    
    void insert(const Station& station);
    bool search(int id, Station& result) const;
    bool remove(int id);
    void clear();
    
    QVector<int> inOrderTraversal() const;
    QVector<int> preOrderTraversal() const;
    QVector<int> postOrderTraversal() const;
    QVector<Station> getAllStations() const;
    
    bool exportTraversals(const QString& filename) const;
    bool isEmpty() const;
    int getSize() const;
    
private:
    TreeNode* m_root;
    int m_size;
    
    TreeNode* insertRecursive(TreeNode* node, const Station& station);
    TreeNode* searchRecursive(TreeNode* node, int id) const;
    TreeNode* removeRecursive(TreeNode* node, int id, bool& success);
    TreeNode* findMin(TreeNode* node) const;
    void clearRecursive(TreeNode* node);
    
    void inOrderRecursive(TreeNode* node, QVector<int>& result) const;
    void preOrderRecursive(TreeNode* node, QVector<int>& result) const;
    void postOrderRecursive(TreeNode* node, QVector<int>& result) const;
    void getAllStationsRecursive(TreeNode* node, QVector<Station>& result) const;
};

#endif // BINARYSEARCH_H
