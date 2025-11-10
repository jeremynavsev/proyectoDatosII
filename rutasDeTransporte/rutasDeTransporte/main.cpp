#include "views/MainWindow.h"
#include "TreeController.h"
#include "GraphController.h"
#include "FileController.h"
#include "BinarySearch.h"
#include "Graph.h"
#include "ReportManager.h"
#include "Station.h"
#include "Edge.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Register custom types for Qt signals/slots
    qRegisterMetaType<Station>("Station");
    qRegisterMetaType<Edge>("Edge");
    qRegisterMetaType<QVector<Station>>("QVector<Station>");
    qRegisterMetaType<QVector<Edge>>("QVector<Edge>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QPair<int,int>>("QPair<int,int>");
    qRegisterMetaType<QVector<QPair<int,int>>>("QVector<QPair<int,int>>");
    
    // Create data structures
    BinarySearchTree* bst = new BinarySearchTree();
    Graph* graph = new Graph();
    ReportManager* reportManager = new ReportManager();
    
    // Create controllers
    TreeController* treeController = new TreeController(bst, reportManager);
    GraphController* graphController = new GraphController(graph, reportManager);
    FileController* fileController = new FileController();
    fileController->setTree(bst);
    fileController->setGraph(graph);
    fileController->setReportManager(reportManager);
    fileController->setDataPath("data/");
    
    // Create main window
    MainWindow mainWindow(treeController, graphController, fileController);
    
    // Load initial data
    fileController->loadAll();
    graphController->loadMap();
    
    // Show window
    mainWindow.show();
    
    int result = app.exec();
    
    // Cleanup
    delete treeController;
    delete graphController;
    delete fileController;
    delete bst;
    delete graph;
    delete reportManager;
    
    return result;
}