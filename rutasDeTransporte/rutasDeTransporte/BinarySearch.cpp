#include "BinarySearch.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>

BinarySearchTree::BinarySearchTree() : m_root(nullptr), m_size(0) {}

BinarySearchTree::~BinarySearchTree() { clear(); }

void BinarySearchTree::insert(const Station& station) {
    m_root = insertRecursive(m_root, station);
}

TreeNode* BinarySearchTree::insertRecursive(TreeNode* node, const Station& station) {
    if (node == nullptr) {
        m_size++;
        return new TreeNode(station);
    }
    if (station < node->data) {
        node->left = insertRecursive(node->left, station);
    } else if (station > node->data) {
        node->right = insertRecursive(node->right, station);
    }
    return node;
}

bool BinarySearchTree::search(int id, Station& result) const {
    TreeNode* found = searchRecursive(m_root, id);
    if (found) {
        result = found->data;
        return true;
    }
    return false;
}

TreeNode* BinarySearchTree::searchRecursive(TreeNode* node, int id) const {
    if (node == nullptr || node->data.getId() == id) return node;
    if (id < node->data.getId()) return searchRecursive(node->left, id);
    return searchRecursive(node->right, id);
}

bool BinarySearchTree::remove(int id) {
    bool success = false;
    m_root = removeRecursive(m_root, id, success);
    if (success) m_size--;
    return success;
}

TreeNode* BinarySearchTree::removeRecursive(TreeNode* node, int id, bool& success) {
    if (node == nullptr) {
        success = false;
        return nullptr;
    }
    if (id < node->data.getId()) {
        node->left = removeRecursive(node->left, id, success);
    } else if (id > node->data.getId()) {
        node->right = removeRecursive(node->right, id, success);
    } else {
        success = true;
        if (node->left == nullptr) {
            TreeNode* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            TreeNode* temp = node->left;
            delete node;
            return temp;
        }
        TreeNode* minRight = findMin(node->right);
        node->data = minRight->data;
        node->right = removeRecursive(node->right, minRight->data.getId(), success);
    }
    return node;
}

TreeNode* BinarySearchTree::findMin(TreeNode* node) const {
    while (node->left != nullptr) node = node->left;
    return node;
}

void BinarySearchTree::clear() {
    clearRecursive(m_root);
    m_root = nullptr;
    m_size = 0;
}

void BinarySearchTree::clearRecursive(TreeNode* node) {
    if (node != nullptr) {
        clearRecursive(node->left);
        clearRecursive(node->right);
        delete node;
    }
}

QVector<int> BinarySearchTree::inOrderTraversal() const {
    QVector<int> result;
    inOrderRecursive(m_root, result);
    return result;
}

void BinarySearchTree::inOrderRecursive(TreeNode* node, QVector<int>& result) const {
    if (node != nullptr) {
        inOrderRecursive(node->left, result);
        result.append(node->data.getId());
        inOrderRecursive(node->right, result);
    }
}

QVector<int> BinarySearchTree::preOrderTraversal() const {
    QVector<int> result;
    preOrderRecursive(m_root, result);
    return result;
}

void BinarySearchTree::preOrderRecursive(TreeNode* node, QVector<int>& result) const {
    if (node != nullptr) {
        result.append(node->data.getId());
        preOrderRecursive(node->left, result);
        preOrderRecursive(node->right, result);
    }
}

QVector<int> BinarySearchTree::postOrderTraversal() const {
    QVector<int> result;
    postOrderRecursive(m_root, result);
    return result;
}

void BinarySearchTree::postOrderRecursive(TreeNode* node, QVector<int>& result) const {
    if (node != nullptr) {
        postOrderRecursive(node->left, result);
        postOrderRecursive(node->right, result);
        result.append(node->data.getId());
    }
}

QVector<Station> BinarySearchTree::getAllStations() const {
    QVector<Station> result;
    getAllStationsRecursive(m_root, result);
    return result;
}

void BinarySearchTree::getAllStationsRecursive(TreeNode* node, QVector<Station>& result) const {
    if (node != nullptr) {
        getAllStationsRecursive(node->left, result);
        result.append(node->data);
        getAllStationsRecursive(node->right, result);
    }
}

bool BinarySearchTree::exportTraversals(const QString& filename) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "=== RECORRIDOS DEL ÁRBOL ===" << "\n\n";
    
    QVector<int> inOrder = inOrderTraversal();
    out << "In-Order: ";
    for (int i = 0; i < inOrder.size(); ++i) {
        out << inOrder[i];
        if (i < inOrder.size() - 1) out << ", ";
    }
    out << "\n\n";
    
    QVector<int> preOrder = preOrderTraversal();
    out << "Pre-Order: ";
    for (int i = 0; i < preOrder.size(); ++i) {
        out << preOrder[i];
        if (i < preOrder.size() - 1) out << ", ";
    }
    out << "\n\n";
    
    QVector<int> postOrder = postOrderTraversal();
    out << "Post-Order: ";
    for (int i = 0; i < postOrder.size(); ++i) {
        out << postOrder[i];
        if (i < postOrder.size() - 1) out << ", ";
    }
    out << "\n";
    
    file.close();
    return true;
}

bool BinarySearchTree::isEmpty() const { return m_root == nullptr; }

int BinarySearchTree::getSize() const { return m_size; }
