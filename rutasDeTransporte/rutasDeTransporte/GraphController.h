#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include <QObject>
#include <QVector>
#include <QPair>
#include "Graph.h"
#include "Station.h"
#include "Edge.h"
#include "ReportManager.h"

class GraphController : public QObject {
    Q_OBJECT
    
public:
    explicit GraphController(Graph* graph, ReportManager* reportManager, QObject* parent = nullptr);
    ~GraphController();
    
    Graph* getGraph();
    ReportManager* getReportManager();
    
public slots:
    void addEdge(int from, int to, double weight);
    void removeEdge(int from, int to);
    void markEdgeClosed(int from, int to);
    void loadMap();
    
    void runBFS(int origin, int destination);
    void runDFS(int origin, int destination);
    void runDijkstra(int origin, int destination);
    void runFloydWarshall(int origin, int destination);
    void runKruskal();
    void runPrim();
    
    void generateReport();
    
signals:
    void connectionAdded(int from, int to, double weight, bool success);
    void connectionRemoved(int from, int to, bool success);
    void closureMarked(int from, int to, bool closed);
    void pathFound(const QString& algorithm, const QVector<int>& path, double cost);
    void pathNotFound(const QString& algorithm);
    void mstFound(const QString& algorithm, const QVector<QPair<int,int>>& edges, double totalCost);
    void mapLoaded(const QVector<Station>& stations, const QVector<Edge>& edges);
    void reportGenerated(bool success);
    void errorOccurred(const QString& message);
    
private:
    Graph* m_graph;
    ReportManager* m_reportManager;
    
    void addReportEntry(const QString& algorithm, int origin, int destination, 
                        const QVector<int>& path, double cost);
    
    double calculatePathCost(const QVector<int>& path);
    
    QVector<int> bfsSearch(int origin, int destination);
    QVector<int> dfsSearch(int origin, int destination);
    QVector<int> dijkstraSearch(int origin, int destination, double& cost);
    QVector<int> floydWarshallSearch(int origin, int destination, double& cost);
    QVector<QPair<int,int>> kruskalMST(double& totalCost);
    QVector<QPair<int,int>> primMST(double& totalCost);
};

#endif // GRAPHCONTROLLER_H
