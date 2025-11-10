#ifndef TREECONTROLLER_H
#define TREECONTROLLER_H

#include <QObject>
#include "BinarySearch.h"
#include "Station.h"
#include "ReportManager.h"

/**
 * @brief Controller for Binary Search Tree operations
 */
class TreeController : public QObject {
    Q_OBJECT
    
public:
    explicit TreeController(BinarySearchTree* tree, ReportManager* reportManager, QObject* parent = nullptr);
    ~TreeController();
    
    BinarySearchTree* getTree();
    
public slots:
    void insertStation(int id, const QString& name);
    void deleteStation(int id);
    void searchStation(int id);
    void performInOrderTraversal();
    void performPreOrderTraversal();
    void performPostOrderTraversal();
    void exportTraversals(const QString& filename);
    void clearTree();
    
signals:
    void stationInserted(int id, const QString& name, bool success);
    void stationDeleted(int id, bool success);
    void stationFound(const Station& station);
    void stationNotFound(int id);
    void traversalResult(const QString& type, const QVector<int>& ids);
    void traversalsExported(bool success, const QString& filename);
    void errorOccurred(const QString& message);
    
private:
    BinarySearchTree* m_tree;
    ReportManager* m_reportManager;
};

#endif // TREECONTROLLER_H
